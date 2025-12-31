#include "packet_generator.h"
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
#include "../common/tx_fifo.h"

// LOG_MODULE_REGISTER(fsm, LOG_LEVEL_DBG);
LOG_MODULE_REGISTER(packet_generator);

static inline struct TXParams* get_global_tp(void) {
    return get_TP();
}

void sendBeacon(void)
{
    LOG_INF("Sending Beacon");
    // Setting the transmission parameters in both layers
    struct TXParams *tp = get_global_tp();
    tp->isFT = true;
    tp->SnetworkID = (networks[0].networkID >> 24) & 0xFF; // For PHY Layer
    tp->S_LRDID = networks[0].own_LRDID;           // For storing, later use
    tp->networkID = networks[0].networkID; 
    // LOG_INF("S_SRDID: %x", tp->S_LRDID);
    tp->S_SRDID = get_rd_id();           // For MAC Layer
    tp->Ring_Level = RING_LEVEL;

    // LOG_INF("S_LRDID: %x", tp->S_LRDID);
    // LOG_INF("Network ID: %x", tp->networkID);
    // LOG_INF("S_SRDID: %x", tp->S_SRDID);

    // Check if it is trying to give coverage
    if(!IS_GATEWAY & is_associated()){
        tp->isGW = true;
    }

    // display_update_upper_text("Sending");
    // display_update_lower_text("Beacon...");

    static struct AssocBeaconMessage abm;
    txAssocBeacon(&abm, tp); 

    // Transmitting beacon
    tx_fifo_put(abm.message, 22, (uint8_t *)&abm.phyheader, 5, 0);

    // Disable the filter to listen any request
    disable_rx_filter();

    return;

}

void sendAssocReq(void)
{
    // timers_stop(ISOLATION_TIMER);

    // Setting the transmission parameters in both layers
    struct TXParams *tp = get_global_tp();
    tp->isFT = false;
    tp->SnetworkID = networks[network_index-1].networkID & 0xFF; // For PHY Layer
    tp->S_SRDID = get_rd_id();                                   // For PHY Layer
    tp->S_LRDID = networks[network_index-1].own_LRDID;           // For storing, later use

    tp->networkID = networks[network_index-1].networkID;         // For MAC Layer
    tp->D_LRDID = networks[network_index-1].network_FT;          // For MAC Layer
    tp->D_SRDID = networks[network_index-1].network_FT >> 16;    // For PHY Layer

    // display_update_upper_text("Sending");
    // display_update_lower_text("Assoc Req");

    //Transmitting Assoc Req
    static struct AssocReqMessage aRm;
    txAssocReq(&aRm, tp);

    // printk("Associating to network %x\n", tp->SnetworkID);

    tx_fifo_put(aRm.message, 26, (uint8_t *)&aRm.phyheader, 5, 0);

    // Enable the filter (waiting for a response from a specific device)
    // enable_rx_filter(tp->SnetworkID, tp->D_SRDID);
    // timers_start(ASSOC_MSG_TIMER, K_MSEC(5000), 2);

    return;
}

void sendAssocResp(void)
{
    LOG_INF("Sending Assoc Resp");
    
    struct TXParams *tp = get_global_tp();

    //// Setting the transmission parameters 
    // Set during Req reception

    // display_update_upper_text("Sending");
    // display_update_lower_text("Assoc Resp");

    static struct AssocRespMessage arm;
    txAssocResp(&arm, tp);

    //Transmitting Assoc Resp
    // modem_tx(&arm.message, 36, &arm.phyheader, 5, 0);
    tx_fifo_put(arm.message, 36, (uint8_t *)&arm.phyheader, 5, 0);
    LOG_INF("FT association success");

    // Enable the filter (for network only)
    // enable_rx_filter(tp->SnetworkID, 0);
    
    if(!associated)
    {
        set_associated(true);
        return;
    }

    
    // Configure timer to repeat beacon transmission (DISABLE TO CREATE BRANCHES);
    // timers_start(BEACON_TIMER, K_MSEC(2000+generateRandomNumber(BEACON_PERIOD)), 2);

    if(IS_GATEWAY)
    {
        // app_set_next_state(FT_OPERATIONS);
    }
    else
    {
        // Next State is IDLE
        disable_rx_filter();
        // app_set_next_state(FT_OPERATIONS);
    }

    return;
}

void sendData(void)
{
    // LOG_INF("Preparing Data to send");

    // display_update_upper_text("Sending");
    // display_update_lower_text("Data...");

    struct TXParams *tp = get_global_tp();
    networks[1].SFN = tp->SFN;
    tp->SFN = tp->SFN;
    tp->D_SRDID = (networks[1].list_LRDID[0]); // For PHY Layer
    tp->SFN = get_SFN(0);

    LOG_INF("Preparing Data to send with SFN: %d", tp->SFN);

    static struct DataMessage dm;

    if(!app_tx_buffer_available())
    {
        LOG_WRN("Buffer empty, no data to send");
        return;
    }
    else
    {
        int a = app_tx_buffer_read(dm.data, 693);
        if(a <= 0) 
        {
            LOG_INF("No data available in application TX buffer to send");
            return;
        }

    }

    int schedule = (tp->SFN * SWITCH_DELAY);
    if(tp->SFN == 0)    schedule = 90;

    txData(&dm, tp);
    tx_fifo_put(dm.payload, 693, (uint8_t *)&dm.phyheader, 5, schedule);
    
    timers_stop(DATA_WIN);
    app_on_timer_event(&(struct timer_event){.type = DATA_WIN, .priority = 1});

    return;
}

void sendBroadcastIndication(void)
{
    // Stopping other timers

    LOG_INF("Sending Broadcast Indication");
    struct TXParams *tp = get_global_tp();


    static struct BroIndIE bie;
    tp->D_SRDID = 0xFFFF;        // Sent to all devices for all to answer
    txBroIndIE(&bie, tp);
    tx_fifo_put(bie.payload, 7, &bie.phyheader, 5, 0);

    disable_rx_filter();

    return;
}

void sendKeepAliveIE(void)
{
    LOG_INF("Sending Keep Alive IE");
    struct TXParams *tp = get_global_tp();

    // k_msleep(200+networks[1].SF1N*1500);                             // To modify with dynamic answers depending on the network

    // LOG_INF("Sending KA IE");
    
    //// Scheduled Access to answer KeepAlive Message
    // LOG_INF("SFN: %d", networks[1].SFN);
    

    static struct KAm kam;
    tp->D_LRDID = networks[1].network_FT;
    tp->D_SRDID = (networks[1].network_FT >> 16) & 0xFFFF;
    tp->SFN = networks[1].SFN;
    LOG_INF("Packet to: %x", tp->D_LRDID);
    LOG_INF("Packet from LRDID: %x", tp->S_LRDID);
    LOG_INF("SNFN to use: %d", tp->SFN);

    int schedule = (tp->SFN * SWITCH_DELAY + 100);
    if(tp->SFN == 0)    schedule = 0;

    if(networks[0].n_devices < 1)set_isFT(false); // If no devices, do not be FT

    if(is_isFT())
    {

        getNetworkInfo(&kam.message);
        txKAm_IE(&kam, tp);
        modem_tx(&kam.payload, 50, &kam.phyheader, 10, schedule);
    }
    else
    {
        LOG_INF("Not an FT, ignoring...");
        LOG_INF("Sending KA IE without data");
        txKAm_IE(&kam, tp);
        modem_tx(&kam.payload, 4, &kam.phyheader, 10, schedule);

    }
    
    // app_set_next_state(WAIT_RX);
    app_set_next_state(WAIT_BROADCAST_IND);

    return;
    
}

void sendDummyData(void)
{
    struct TXParams *tp = get_global_tp();

    tp->SnetworkID = 0xFF;
    tp->S_SRDID = 0xFFFF;
    tp->D_SRDID = 0xFFFF;
    tp->sequenceNumber = 0;
    tp->reset = 0;


    static struct DataMessage dm;

    // Fill the data message with dummy data
    for (size_t i = 0; i < 693; i++) {
        dm.data[i] = (uint8_t)(i & 0xFF);
    }


    int schedule = (tp->SFN * SWITCH_DELAY + 100);
    if(tp->SFN == 0)    schedule = 0;

    txData(&dm, tp);

    tx_fifo_put(&dm.payload, 693, &dm.phyheader, 5, 0);

    return;
}