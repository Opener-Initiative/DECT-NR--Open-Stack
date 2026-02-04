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
// #include "../drivers/display_handler.h"
#include "../common/data_buffers.h"
#include "../app/app_state.h"
#include "../app/packet_generator.h"

int bytes_processed = 0;
bool netCheck = false;

LOG_MODULE_REGISTER(packet_processor, LOG_LEVEL_NONE);


/// Packet processing function called from rx_consumer_thread
void process_rx_packet(const uint8_t *_rxData, size_t len, uint16_t transmitter_srdid, uint8_t networkID)
{

    LOG_WRN("Processing received packet");
    LOG_INF("Transmitter SRDID: %x", transmitter_srdid);
    LOG_INF("Network ID: %x", networkID);

    static mlcf_a_t mac_header;

    if (len < 1) {
        LOG_INF("Empty packet");
        return;
    }
    else
    {
        memcpy(&mac_header.mlcf_a, &_rxData[0], 1);
        get_mlcf_a_rev(&mac_header);
        bytes_processed += 1;
        // LOG_INF("Packet received (len=%d)", len);

        // printk("Packet received (hex): ");
        // for (size_t i = 0; i < len && i < 32; i++) {  // Limit to 32 bytes to avoid overflow
        //     printk("%02X ", _rxData[i]);
        // }
        // printk("\n");
        // k_msleep(140);
    }
    LOG_INF("MAC Header Type: %d", mac_header.HeaderType);
    switch (mac_header.HeaderType)
    {
    case 0b0000:
        LOG_INF("DATA MAC PDU received");
        // static mac_common_header_t mac_data_header;
        bytes_processed += 2;

        break;

    case 0b0001:
        LOG_INF("BEACON MAC PDU received");

        static mlcf_b_2t mac_beacon_header;
        memcpy(&mac_beacon_header.mlcf_b_2, &_rxData[1], 7);
        get_mlcf_b_2_rev(&mac_beacon_header);
        bytes_processed += 7;

        //// Check if network is already registered
        LOG_INF("Network ID: %x", mac_beacon_header.NetworkID);
        LOG_INF("Transmitter Address: %x", mac_beacon_header.TransmitterAddress);
        uint32_t nID = ((mac_beacon_header.NetworkID << 8) & 0xFFFFFF00) | (networkID & 0x000000FF);;
        LOG_INF("Constructed Network ID: %x", nID);
        netCheck = networkCheck(mac_beacon_header.NetworkID, mac_beacon_header.TransmitterAddress);

        break;

    case 0b0010:
        LOG_INF("Unicast Header received");
        bytes_processed += 10;
        // static mac_common_header_t mac_unicast_header;
        break;

    case 0b0011:
        LOG_INF("Association Request Header received");
        // static mac_common_header_t mac_assoc_req_header;
        break;

    case 0b1111:
        LOG_INF("Escape");
        break;
    
    default:
        break;
    }

    while(bytes_processed < len)
    {
        LOG_INF("Bytes processed: %d / %d", bytes_processed, len);

        //// MUX Header processing
        if(_rxData[bytes_processed] == 0x00)
        {
            LOG_INF("No MUX Headers present");

            bytes_processed = len;

            break;
        }

        static mlcf_c_1t mac_mux_header;
        memcpy(&mac_mux_header.mlcf_c_1, &_rxData[bytes_processed], 1);
        get_mlcf_c_1_rev(&mac_mux_header);
        LOG_WRN("mac_mux_header.MAC_Ext: %d", mac_mux_header.MAC_Ext);
        LOG_WRN("mac_mux_header.IE_Type: %d", mac_mux_header.IE_Type);

        if(mac_mux_header.MAC_Ext == 0b11)
        {
            LOG_INF("MUX Header Type: Option a & b");

            if(mac_mux_header.Length == 0)
            {
                LOG_INF("Option a");

                switch (mac_mux_header.IE_Type)
                {
                    case 0b00000:
                        LOG_INF("IE Type: Padding IE");
                        break;

                    case 0b00010:
                        LOG_INF("IE Type: Keep Alive IE");
                        LOG_INF("Transmitter SRDID from header: %x", transmitter_srdid);

                        //// Check device
                        // deviceCheck(transmitter_srdid);

                        bytes_processed = len;

                        break;

                    default:
                        LOG_WRN("No IE Type recognized");
                        break;
                }

            }
            else
            {
                LOG_INF("Option b");
            }
        }

        else if(mac_mux_header.MAC_Ext == 0b01 || mac_mux_header.MAC_Ext == 0b10 || mac_mux_header.MAC_Ext == 0b00)
        {
            LOG_INF("MUX Header Type: Option c, d, e & f");
            static mlcf_c_2t mac_mux_header;
            memcpy(&mac_mux_header.mlcf_c_2, &_rxData[bytes_processed], 1);
            get_mlcf_c_2_rev(&mac_mux_header);

            switch (mac_mux_header.IE_Type)
            {
                case 0b001000:
                    LOG_INF("IE Type: Beacon IE");
                    static mlmf_2_t network_beacon_message;
                    memcpy(&network_beacon_message.mlmf_2, &_rxData[bytes_processed], 13);
                    bytes_processed += 13;
                    get_mlmf_2_rev(&network_beacon_message);


                    LOG_INF("Ring Level: %d\n", network_beacon_message.Reserved_1);

                    if(!netCheck){
                        //// Sending Association Request
                        sendAssocReq();
                    }
                    // sendDummyData();

                    break;


                case 0b001010:
                    LOG_INF("IE Type: Association Request Message");
                    static mlmf_4_t assoc_request_message;
                    memcpy(&assoc_request_message.mlmf_4, &_rxData[bytes_processed], 14);
                    bytes_processed += 14;
                    get_mlmf_4_rev(&assoc_request_message);
                    sendAssocResp();

                    break;


                case 0b001011:
                    LOG_INF("IE Type: Association Response Message");
                    static mlmf_4_t assoc_response_message;
                    memcpy(&assoc_response_message.mlmf_4, &_rxData[bytes_processed], 7);
                    bytes_processed += 7;
                    get_mlmf_4_rev(&assoc_response_message);

                    LOG_INF("PT ASSOCIATED SUCCESSFULLY");

                    set_associated(true);
                    timers_stop(ASSOC_WIN);
                    app_on_timer_event(&(struct timer_event){.type = ASSOC_WIN, .priority = 1});

                    // LOG_INF("Assigned LRDID: %x", assoc_response_message.Assigned_LRDID);
                    break;


                case 0b010010:
                    LOG_INF("IE Type: Resource Allocation IE");
                    static mlie_3_t resource_allocation_ie;
                    memcpy(&resource_allocation_ie.mlie_3, &_rxData[bytes_processed], 16);
                    bytes_processed += 16;
                    get_mlie_3_rev(&resource_allocation_ie);

                    LOG_INF("SFN Value: %x", resource_allocation_ie.SFN_Value);
                    break;


                case 0b000011:
                    if(mac_mux_header.MAC_Ext != 0b10)
                    {
                        LOG_WRN("Incorrect MUX Header used for User plane data - flow 1");
                        break;
                    }
                    LOG_INF("IE Type: User plane data - flow 1");
                    // bytes_processed += 3;
                    static mlcf_c_4t mac_mux_header_4;
                    memcpy(&mac_mux_header_4.mlcf_c_4, &_rxData[bytes_processed], 4);
                    bytes_processed += 4;
                    get_mlcf_c_4_rev(&mac_mux_header_4);

                    LOG_INF("Length: %d", mac_mux_header_4.Length);

                    if(!app_rx_buffer_available())
                    {
                        LOG_INF("RX buffer available, writing data...");
                        app_rx_buffer_write(&_rxData[bytes_processed], mac_mux_header_4.Length);
                        bytes_processed += mac_mux_header_4.Length;
                    }
                    else
                    {
                        LOG_INF("RX buffer not available, dropping data...");
                    }

                    //// Process user plane data - flow 1
                    break;


                case 0b010110:
                    LOG_INF("IE Type: Broadcast Indication IE");
                    static mlie_7_t broadcast_indication_ie;
                    memcpy(&broadcast_indication_ie.mlie_7, &_rxData[bytes_processed], 4);
                    bytes_processed += 4;
                    get_mlie_7_rev(&broadcast_indication_ie);

                    // LOG_INF("Broadcast Indication IE received");

                    if(app_tx_buffer_available())
                    {
                        LOG_INF("TX buffer available, sending data...");
                        sendData();
                    }
                    else
                    {
                        LOG_INF("TX buffer not available, sending keep alive IE...");
                        sendKeepAliveIE();
                    }

                    //// Checkings according FSM and generate an answer if needed
                    // sendBroadcastIndication();

                    break;

                
                    


                    




                default:
                    LOG_WRN("No IE Type recognized");
                    bytes_processed = len; // Exit the loop
                    break;
            }
        }
        else
        {
            LOG_WRN("No MUX MAC Extension recognized");
            break;
        }
    }
    bytes_processed = 0;
}

void packet_processor_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    LOG_INF("Packet processor thread started");

    while (1) {

        k_msleep(10);

        // LOG_INF("Waiting for RX packet...");

        /* Wait for a packet from the modem */
        struct rx_packet *pkt = rx_fifo_get(K_FOREVER);

        /* 1. Process at protocol level */
        process_rx_packet(pkt->data, pkt->len, pkt->transmitter_srdid, pkt->networkID);

        /* 2. Free memory */
        rx_fifo_free(pkt);
    }
}

K_THREAD_DEFINE(packet_processor_tid, 4096, packet_processor_thread, NULL, NULL, NULL,
                5, 0, 0);