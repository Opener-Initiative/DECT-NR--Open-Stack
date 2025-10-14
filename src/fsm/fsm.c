#include "fsm.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "../config/device_config.h"
#include "../common/dect_timers.h"
#include "../protocol/procedures.h"
#include "../common/network.h"
#include "../drivers/modem_handler.h"
#include "../common/rx_fifo.h" 
#include "../headers/mac_header_field.h" 
#include "../common/utils.h" 
#include "../app/app_state.h"
#include "../drivers/sensor_htu21d.h"
#include "../common/sfn_allocator.h"
#include "../drivers/display_handler.h"
#include "fsm_event.h"

LOG_MODULE_REGISTER(fsm, LOG_LEVEL_DBG);

static inline struct TXParams* get_global_tp(void) {
    return get_TP();
}

//// Consumer thread for RX packets
static void rx_consumer_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    while (1) {
        struct rx_packet *pkt = rx_fifo_get(K_FOREVER);
        if (!pkt) continue;

        // LOG_INF("rx_consumer: paquete recibido (len=%d)", pkt->len);

    // /* Indicate we are processing a packet so timers with priority != 0 are deferred */
    // timers_set_processing(true);
    // process_rx_packet(pkt->data, pkt->len);
    // timers_set_processing(false);
    // /* After finishing processing, handle any pending timer events */
    // timers_process_pending();

    //// Create an event for the FSM
    struct fsm_event evt = {
        .type = FSM_EVENT_PACKET_RX,
    };
    evt.rx.len = pkt->len > sizeof(evt.rx.data) ? sizeof(evt.rx.data) : pkt->len;
    memcpy(evt.rx.data, pkt->data, evt.rx.len);

    /* Enviamos el evento a la cola de la FSM */
    if (fsm_event_post(&evt) != 0) {
        LOG_WRN("FSM event queue full, dropping packet");
    }


    rx_fifo_free(pkt);
    }
}

/// Thread starting
K_THREAD_DEFINE(rx_consumer_tid, 4096, rx_consumer_thread, NULL, NULL, NULL,
                5, 0, 0);

void rx_consumer_init(void)
{
    LOG_INF("rx_consumer started");
}

/// Packet processing function called from rx_consumer_thread
void process_rx_packet(const uint8_t *_rxData, size_t len)
{
    timers_stop(BEACON_TIMER); 

    static mlcf_a_t mac_header;

    if (len < 1) {
        LOG_INF("Empty packet");
        return;
    }
    else
    {
        memcpy(&mac_header.mlcf_a, &_rxData[0], 1);
        get_mlcf_a_rev(&mac_header);
        // LOG_INF("Paquete recibido (len=%d)", len);

        // printk("Paquete recibido (hex): ");
        // for (size_t i = 0; i < len && i < 32; i++) {  // Limitar a 32 bytes para evitar overflow
        //     printk("%02X ", _rxData[i]);
        // }
        // printk("\n");
    }


    switch (app_get_state()) 
    {
        case 2: // WAIT_BEACON - Listening for Beacons
            LOG_INF("Waiting for beacon...");

            if(mac_header.HeaderType != 1){
                LOG_INF("No Beacon PDU received");
                timers_start(BEACON_TIMER, K_MSEC(BEACON_PERIOD + generateRandomNumber(BEACON_PERIOD)), 2);
                break;
            }

            LOG_INF("Beacon PDU received");

            display_update_upper_text("Waiting");
            display_update_lower_text("Beacon...");
            
            associate:

            // Getting the MAC Beacon Header
            static mlcf_b_2t mac_beacon_header;
            memcpy(&mac_beacon_header.mlcf_b_2, &_rxData[1], 7);
            get_mlcf_b_2_rev(&mac_beacon_header);

            // Getting the MUX Header
            static mlcf_c_2t mac_mux_header_beacon;
            memcpy(&mac_mux_header_beacon.mlcf_c_2, &_rxData[8], 1);
            get_mlcf_c_2_rev(&mac_mux_header_beacon);

            // Getting the MAC Beacon message
            static mlmf_2_t mac_beacon_message;
            memcpy(&mac_beacon_message.mlmf_2, &_rxData[9], 13);
            get_mlmf_2_rev(&mac_beacon_message);

            
            if(is_associated())
            {
                if(mac_beacon_message.Reserved == 0)
                {
                    // Sends a beacon to allow others PT to connect to the gateway
                    app_set_next_state(SEND_BEACON);
                    LOG_INF("Device associated and detected isolated device, sending Beacon...");
                }
                else
                {
                    LOG_INF("Device associated and detected another device FT GW, ignoring...");
                }
            }
            else 
            {
                if(mac_beacon_message.Reserved == 1)
                {
                    LOG_INF("Device not associated and detected a device FT GW, associating...");
                    // The device is not associated and the beacon is from a FT gateway. Associate. Not otherwise

                    if(IS_GATEWAY && USING_GATEWAYS)
                    {
                        LOG_INF("Device is a proper GW, ignoring...");
                        timers_start(BEACON_TIMER, K_MSEC(BEACON_PERIOD + generateRandomNumber(BEACON_PERIOD)), 1);
                        return;
                    }

                    bool netCheck = networkCheck(mac_beacon_header.NetworkID, mac_beacon_header.TransmitterAddress);
                    if(!netCheck){
                        // Sending Association Request
                        app_set_next_state(SEND_ASSOC_REQ);
                    }
                    else
                    {
                        LOG_INF("Network already registered, ignoring...");
                    }
                }
                else
                {
                    LOG_INF("Device not associated and detected another device not GW, ignoring...");
                }
            }

            timers_start(BEACON_TIMER, K_MSEC(BEACON_PERIOD + generateRandomNumber(BEACON_PERIOD)), 1);

            break;


        case 4: // WAIT_ASSOC_REQ - Assoc Req
            LOG_INF("Waiting for Assoc Req...");

            display_update_upper_text("Waiting");
            display_update_lower_text("Assoc Req");

            if(mac_header.HeaderType == 2)
            {
                LOG_INF("Assoc Req received");

                static mlcf_b_3t mac_unicast;
                memcpy(&mac_unicast.mlcf_b_3, &_rxData[1], 10);
                get_mlcf_b_3_rev(&mac_unicast);

                // LOG_INF("Received Transmitter LRDID (Paquete de): %x", mac_unicast.TransmitterAddress); // De quien es el pck
                // LOG_INF("Received Receiver LRDID (Paquete para): %x\n", mac_unicast.ReceiverAddress); // A quien va el pck
                // LOG_INF("MY LRDID: %x\n", networks[0].own_LRDID); // My LRDID

                if(!checkDestination(mac_unicast.ReceiverAddress)){
                    LOG_INF("Package not for me, ignoring...");
                    timers_start(BEACON_TIMER, K_MSEC(BEACON_PERIOD + generateRandomNumber(BEACON_PERIOD)), 1);
                    break;
                }

                // Getting the MUX Header
                static mlcf_c_2t mac_mux_header_assoc_req;
                memcpy(&mac_mux_header_assoc_req.mlcf_c_2, &_rxData[11], 1);
                get_mlcf_c_2_rev(&mac_mux_header_assoc_req);

                // Getting the message
                static mlmf_4_t mac_assoc_message;
                memcpy(&mac_assoc_message.mlmf_4, &_rxData[12], 14);
                get_mlmf_4_rev(&mac_assoc_message);

                // FT do not enables any filtering on association
                // Disable the filter
                disable_rx_filter();

                //Check if requesting PT already exists in the network.

                if(deviceCheck(mac_unicast.TransmitterAddress)){
                    app_set_next_state(SEND_ASSOC_RESP);
                    break;
                }
                

                //// Setting the transmission parameters 
                // IDs
                struct TXParams *tp = get_global_tp();
                tp->S_LRDID = networks[0].own_LRDID;
                tp->D_LRDID = tp->D_LRDID;
                tp->SnetworkID = networks[0].networkID & 0xFF; // For PHY Layer
                tp->S_SRDID = get_rd_id();                          // For PHY Layer
                
                tp->SFN = sfn_alloc_global();
                networks[0].list_LRDID[networks[0].n_devices][2] = tp->SFN; // Save the SFN in the list of devices
                
                networks[0].list_LRDID[networks[0].n_devices][0] = mac_unicast.TransmitterAddress;
                networks[0].list_LRDID[networks[0].n_devices][1] = 3;                             
                tp->D_LRDID = mac_unicast.TransmitterAddress;                                     
                networks[0].n_devices++;                                                          
                // LOG_INF("New device associated: %x\n", networks[0].list_LRDID[networks[0].n_devices-1][0]);

                // Changing to state to Sending Association Response
                app_set_next_state(SEND_ASSOC_RESP);
            }

            // TO REVISE!!!!!
            else if(mac_header.HeaderType == 1)      // Case of trying to be a FT, not priority and my network is empty
            {
                if(networks[0].list_LRDID[0][0] == 0){
                    goto associate;
                }
                else
                {
                    LOG_INF("My network is not empty, cannot associate, ignoring...");
                    timers_start(BEACON_TIMER, K_MSEC(BEACON_PERIOD + generateRandomNumber(BEACON_PERIOD)), 1);
                    break;
                }
            }
            else
            {
                LOG_INF("No Assoc Req PDU received");
                break;
            }
            break;

        case 6: // WAIT_ASSOC_RESP - Assoc Response
            LOG_INF("Waiting for Assoc Resp...");

            if(mac_header.HeaderType != 2){
                LOG_INF("No Assoc Resp PDU received, waiting for Assoc Resp...");
                break;
            }
            display_update_upper_text("Waiting");
            display_update_lower_text("Assoc Resp");

            LOG_INF("Assoc Resp received");

            mlcf_b_3t mac_unicast;
            memcpy(&mac_unicast.mlcf_b_3, &_rxData[1], 10);
            get_mlcf_b_3_rev(&mac_unicast);

            // LOG_INF("Received Receiver LRDID (Paquete para): %x", mac_unicast.ReceiverAddress); 
            // LOG_INF("Received Transmitter LRDID (Paquete de): %x", mac_unicast.TransmitterAddress); 
            // LOG_INF("network_index: %d\n", network_index);
            // LOG_INF("Mi LRDID: %x\n", networks[network_index-1].own_LRDID);

            if(!checkDestination(mac_unicast.ReceiverAddress)){
            LOG_INF("Package not for me, ignoring...");
            return;
            }
            

            // Getting the MUX Header
            mlcf_c_2t mac_mux_header_assoc_resp; 
            memcpy(&mac_mux_header_assoc_resp.mlcf_c_2, &_rxData[11], 1);
            get_mlcf_c_2_rev(&mac_mux_header_assoc_resp);

            // Getting the message
            mlmf_5_t mac_assoc_resp_message;
            memcpy(&mac_assoc_resp_message.mlmf_5, &_rxData[12], 7);
            get_mlmf_5_rev(&mac_assoc_resp_message);

            // Getting the MUX Header for access coordination
            mlcf_c_2t mac_mux_header_1_assoc_resp; 
            memcpy(&mac_mux_header_1_assoc_resp.mlcf_c_2, &_rxData[19], 1);
            get_mlcf_c_2_rev(&mac_mux_header_1_assoc_resp);

            // Getting the message for access coordination
            mlie_3_t mac_resource_allocation;
            memcpy(&mac_resource_allocation.mlie_3, &_rxData[20], 16);
            get_mlie_3_rev(&mac_resource_allocation);

            // printk("mac_resource_allocation[12]: %x\n", mac_resource_allocation.mlie_3[12]);
            // LOG_INF("SFN: %x\n", mac_resource_allocation.SFN_Value);


            // Check if response is ACK or NACK
            // if(mac_assoc_resp_message.ACK_NACK == 1){
            //   LOG_INF("Association Response is ACK");
            //   // Save the LRDID of the device for later use
            //   networks[0].list_LRDID[networks[0].n_devices] = mac_unicast.TransmitterAddress;


            LOG_INF("PT ASSOCIATION SUCCESS");
            display_update_text("Associated");
            k_msleep(100);
            associated = true;
            networks[network_index-1].SFN = mac_resource_allocation.SFN_Value;
            // printk("Stored %d in network %x\n", networks[network_index-1].SFN, network_index-1);

            // for(int i = 0; i < network_index; i++){
            // printk("Network ID: %x\n", networks[i].networkID);
            // printk("FT: %x\n", networks[i].network_FT);
            // printk("LRDID: %x\n", networks[i].own_LRDID);
            // }

            // Enable the filter to only receive packets from associated devices
            // enable_rx_filter();

            // Disable message timer
            timers_stop(ASSOC_MSG_TIMER);

            // Changing to state IDLE
            app_set_next_state(WAIT_DATA);

            break;

        case 8: // WAIT_DATA - Data
            
            LOG_INF("Checking for data packet");

            // LOG_INF("Datos recibidos:");
            // for (int i = 0; i < len; i++) {
            //   printk("%02X ", _rxData[i]);
            // }
            // printk("\n");

            // LOG_INF("Header Type: %d", mac_header.HeaderType);

            if(mac_header.HeaderType != 0){
                LOG_INF("No data packet received, waiting for data packet...");
                break;
            }

            // Getting the MAC Common Header (DATA MAC PDU Header)
            mlcf_b_1t mac_data_header1;
            memcpy(&mac_data_header1.mlcf_b_1, &_rxData[1], 2);
            get_mlcf_b_1_rev(&mac_data_header1);

            // Getting the MUX Header
            mlcf_c_4t mac_mux_header1;
            memcpy(&mac_mux_header1.mlcf_c_4, &_rxData[3], 4);
            get_mlcf_c_4_rev(&mac_mux_header1);
            // LOG_INF("MAC MUX Header Length: %d", mac_mux_header1.Length);

            // Getting the MAC DATA message
            static struct DataMessage dm_rx;
            size_t copy_len = mac_mux_header1.Length;
            if (copy_len > sizeof(dm_rx.payload)) copy_len = sizeof(dm_rx.payload);
            memcpy(dm_rx.payload, &_rxData[7], copy_len);

            uint8_t *app = dm_rx.payload;
            size_t app_len = copy_len;

            if (app && app_len >= 16) {
                double temp_d = 0.0, hum_d = 0.0;
                memcpy(&temp_d, app + 0, sizeof(double));
                memcpy(&hum_d, app + sizeof(double), sizeof(double));

                //// Check ranges
                if ((temp_d > -50.0 && temp_d < 100.0) && (hum_d >= 0.0 && hum_d < 120.0)) {
                    // LOG_INF("Parsed application payload as doubles: Temp=%.2f C, Hum=%.2f %%", temp_d, hum_d);
                    printk("Temperature: %.2f C\n", temp_d);
                    printk("Humidity: %.2f %%\n", hum_d);

                    //// Update display to show values
                    static char upper_text[48];
                    static char lower_text[48];
                    snprintk(upper_text, sizeof(upper_text), "Temp: %.2f C", temp_d);
                    snprintk(lower_text, sizeof(lower_text), "Hum: %.2f %%", hum_d);
                    display_update_set_upper_text(upper_text);
                    display_update_set_lower_text(lower_text);
                } else {
                    app = app; 
                }
            }
            else
            {
                LOG_INF("Data in the packet is not valid");
            }

            app_set_next_state(8);

            break;


        case 10:

            LOG_INF("Device is associated, stand by...");
            break;
            

        default:
            LOG_INF("Estado desconocido o paquete no esperado en estado %d", app_get_state());
            break;
    }
}


void sendBeacon(void)
{
    // Setting the transmission parameters in both layers
    struct TXParams *tp = get_global_tp();
    tp->isFT = true;
    tp->SnetworkID = networks[0].networkID & 0xFF; // For PHY Layer
    tp->S_LRDID = networks[0].own_LRDID;           // For storing, later use
    tp->networkID = networks[0].networkID;         // For MAC Layer
    tp->Ring_Level = RING_LEVEL;

    // Check if it is trying to give coverage
    if(!IS_GATEWAY & is_associated()){
        tp->isGW = true;
    }

    display_update_upper_text("Sending");
    display_update_lower_text("Beacon...");

    static struct AssocBeaconMessage abm;
    txAssocBeacon(&abm, tp); 

    // Transmitting beacon
    modem_tx(abm.message, 22, (uint8_t *)&abm.phyheader, 5);

    // Disable the filter to listen any request
    disable_rx_filter();

    timers_start(BEACON_TIMER, K_MSEC(5000+generateRandomNumber(5000)), 2);

    return;

}

void sendAssocReq(void)
{
    timers_stop(ISOLATION_TIMER);

    // Setting the transmission parameters in both layers
    struct TXParams *tp = get_global_tp();
    tp->isFT = false;
    tp->SnetworkID = networks[network_index-1].networkID & 0xFF; // For PHY Layer
    tp->S_SRDID = get_rd_id();                                   // For PHY Layer
    tp->S_LRDID = networks[network_index-1].own_LRDID;           // For storing, later use

    tp->networkID = networks[network_index-1].networkID;         // For MAC Layer
    tp->D_LRDID = networks[network_index-1].network_FT;          // For MAC Layer
    tp->D_SRDID = networks[network_index-1].network_FT >> 16;    // For PHY Layer

    display_update_upper_text("Sending");
    display_update_lower_text("Assoc Req");

    //Transmitting Assoc Req
    static struct AssocReqMessage aRm;
    txAssocReq(&aRm, tp);

    // printk("Associating to network %x\n", tp->SnetworkID);

    modem_tx(aRm.message, 26, (uint8_t *)&aRm.phyheader, 10);

    // Enable the filter (waiting for a response from a specific device)
    enable_rx_filter(tp->SnetworkID, tp->D_SRDID);
    timers_start(ASSOC_MSG_TIMER, K_MSEC(5000), 2);

    return;
}

void sendAssocResp(void)
{
    LOG_INF("Sending Assoc Resp");
    
    struct TXParams *tp = get_global_tp();

    //// Setting the transmission parameters 
    // Set during Req reception

    display_update_upper_text("Sending");
    display_update_lower_text("Assoc Resp");

    static struct AssocRespMessage arm;
    txAssocResp(&arm, tp);

    //Transmitting Assoc Resp
    modem_tx(&arm.message, 36, &arm.phyheader, 5);
    LOG_INF("FT association success");

    // Enable the filter (for network only)
    enable_rx_filter(tp->SnetworkID, 0);
    
    if(!associated)
    {
        app_set_next_state(FT_OPERATIONS);
        set_associated(true);
        return;
    }

    
    // Configure timer to repeat beacon transmission;
    timers_start(BEACON_TIMER, K_MSEC(2000+generateRandomNumber(BEACON_PERIOD)), 2);

    app_set_next_state(FT_OPERATIONS);

    return;
}

void sendData(void)
{
    // LOG_INF("Preparing Data to send");

    display_update_upper_text("Sending");
    display_update_lower_text("Data...");

    struct TXParams *tp = get_global_tp();
    networks[1].SFN = tp->assignated_SFN;
    tp->SFN = tp->assignated_SFN;
    tp->D_SRDID = (networks[1].list_LRDID[0]); // For PHY Layer

    static struct DataMessage dm;

    // Fill the data message with data
    if(sensor_htu21d_is_ready())
    {
        double temp = get_htu21d_temperature();
        double hum = get_htu21d_humidity();
        memcpy(dm.data, &temp, sizeof(double));
        memcpy(dm.data + sizeof(double), &hum, sizeof(double));
    }
    else
    {
        double temp = 30.5;
        double hum = 24.1;
        memcpy(dm.data, &temp, sizeof(double));
        memcpy(dm.data + sizeof(double), &hum, sizeof(double));
    }

    // LOG_INF("Sending both temperature and humidity: %.2f C, %.2f %%", temp, hum);

    txData(&dm, tp);

    modem_tx(&dm.payload, 50, &dm.phyheader, 10);


    app_set_next_state(SEND_DATA);

    k_msleep(5000);

    return;
}

int ftOperations(void)
{
    /// Internal logic for FT operation. Transient state.

    if(!associated && IS_GATEWAY)   /// Device not associate and should be. Keep transmitting beacons
    {
      LOG_INF("Device GW not associated, waiting for devices to associate");
      return SEND_BEACON;
    }
    else if(associated && IS_GATEWAY)   /// Device associated and should be. Operate
    {
        return SEND_DATA;
    }
    else        /// Device not connected. Return to connect
    {
        if(!associated)
        {
            LOG_INF("UNEXPECTED BEHAVIOR. Revise FSM");
            return SEND_BEACON;
        } 
    }
}



