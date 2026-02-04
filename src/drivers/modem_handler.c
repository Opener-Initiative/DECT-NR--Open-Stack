#include "modem_handler.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <modem/nrf_modem_lib.h>
#include <nrf_modem_dect_phy.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "../headers/physical_header_field.h"
#include "../common/rx_fifo.h"
#include "../config/device_config.h"
#include "../common/utils.h"
#include "../protocol/procedures.h"
/* Consult app state to decide whether to enqueue RX packets */
#include "../app/app_state.h"
#include "../common/data_buffers.h"
#include "../common/modem_operator.h"

LOG_MODULE_REGISTER(modem_h);

/* Semaphore to serialize async modem calls (copied from main.c) */
K_SEM_DEFINE(modem, 0, 1);
K_SEM_DEFINE(no_modem, 0, 1);
/* Flag set on fatal modem error; marked unused to avoid warnings until used */
static bool modem_fatal_error_flag __attribute__((unused));
static uint64_t modem_time;

static int rxHandle __attribute__((unused)) = 31400;
static int txHandle = 1;
struct nrf_modem_dect_phy_rx_params rxOpsParams = {0};
union nrf_modem_dect_phy_hdr phyHeader;
static uint8_t _txData[DATA_LEN];
static uint8_t _rxData[DATA_LEN] __attribute__((unused));

static struct TXParams tp;
static bool setContinuousRx = false;


/* local state variables (adapted from main.c) */
static int crc_errors __attribute__((unused)) = 0;
static float rssi_average = 0;
static int n = 0;
static int8_t rssi_measure __attribute__((unused)) = 0;
plcf_10_t rx_phy_hdr[5];
plcf_20_t rx_phy_hdr_2[10];
uint16_t transmitter_srdid;
uint8_t networkID;

bool modem_free = true;


static int32_t calcRSSI(int16_t recrssi, int is_success)
{
    float resp = -20 - ((-recrssi - 1) * 0.5f);
    if (is_success) {
        n++;
        float new_average = rssi_average + (resp - rssi_average) / n;
        rssi_average = new_average;
    }
    return (int32_t)resp;
}


/* Callback after init operation. */
static void on_init(const struct nrf_modem_dect_phy_init_event *evt)
{
	if (evt->err) {
    LOG_ERR("INIT FAILED");
    LOG_INF("Init failed\n");
  modem_fatal_error_flag = true;
		return;
	}

	k_sem_give(&modem);
}

/* Callback after deinit operation. */
static void on_deinit(const struct nrf_modem_dect_phy_deinit_event *evt)
{
	if (evt->err) {
		LOG_ERR("Deinit failed, err %d", evt->err);
		return;
	}

	k_sem_give(&no_modem);
}

static void on_activate(const struct nrf_modem_dect_phy_activate_event *evt)
{
	if (evt->err) {
		LOG_ERR("Activate failed, err %d", evt->err);
  modem_fatal_error_flag = true;
		return;
	}

	k_sem_give(&modem);
}

static void on_deactivate(const struct nrf_modem_dect_phy_deactivate_event *evt)
{

	if (evt->err) {
		LOG_ERR("Deactivate failed, err %d", evt->err);
		return;
	}

	k_sem_give(&no_modem);
}

static void on_configure(const struct nrf_modem_dect_phy_configure_event *evt)
{
	if (evt->err) {
		LOG_ERR("Configure failed, err %d", evt->err);
		return;
	}

	k_sem_give(&modem);
}

/* Callback after link configuration operation. */
static void on_link_config(const struct nrf_modem_dect_phy_link_config_event *evt)
{
	LOG_DBG("link_config cb time %"PRIu64" status %d", modem_time, evt->err);
}

static void on_radio_config(const struct nrf_modem_dect_phy_radio_config_event *evt)
{
	if (evt->err) {
		LOG_ERR("Radio config failed, err %d", evt->err);
		return;
	}

	k_sem_give(&modem);
}

/* Callback after capability get operation. */
static void on_capability_get(const struct nrf_modem_dect_phy_capability_get_event *evt)
{
	LOG_DBG("capability_get cb time %"PRIu64" status %d", modem_time, evt->err);

    LOG_INF("DECT PHY Capabilities:");
    LOG_INF("DECT Version: %d", evt->capability);
}

static void on_bands_get(const struct nrf_modem_dect_phy_band_get_event *evt)
{
	LOG_DBG("bands_get cb status %d", evt->err);
}

static void on_latency_info_get(const struct nrf_modem_dect_phy_latency_info_event *evt)
{
	LOG_DBG("latency_info_get cb status %d", evt->err);
}

/* Callback after time query operation. */
static void on_time_get(const struct nrf_modem_dect_phy_time_get_event *evt)
{
	LOG_DBG("time_get cb time %"PRIu64" status %d", modem_time, evt->err);
    LOG_INF("Current time: %d", modem_time);
}

static void on_cancel(const struct nrf_modem_dect_phy_cancel_event *evt)
{
	LOG_DBG("on_cancel cb status %d", evt->err);
	k_sem_give(&modem);
}

/* Operation complete notification. */
static void on_op_complete(const struct nrf_modem_dect_phy_op_complete_event *evt)
{
	LOG_DBG("op_complete cb time %"PRIu64" status %d", modem_time, evt->err);
    // LOG_INF("Operation complete, handle %d, err %x", evt->handle, evt->err);
	k_sem_give(&modem);
    modem_op_complete();
}

/* Physical Control Channel reception notification. */
static void on_pcc(const struct nrf_modem_dect_phy_pcc_event *evt)
{
//   LOG_INF("pcc_cb phy_header_valid %d rssi_2 %d", evt->header_status, evt->rssi_2);
  
  memcpy(rx_phy_hdr->plcf, &evt->hdr, 5);
  // printk("Net ID: %x\n", hdr->type_1[1]);

  get_plcf_1_rev(rx_phy_hdr);
  // printk("ShortNetworkID: %x\n", rx_phy_hdr->ShortNetworkID);
  
  if(rx_phy_hdr->HeaderFormat == 0){
    // LOG_INF("PCC received, type 0, Short Header");
    transmitter_srdid = rx_phy_hdr->TransmitterIdentity;
    networkID = rx_phy_hdr->ShortNetworkID;
    // LOG_INF("Transmitter ID: %x\n", transmitter_srdid);
  }
  else if(rx_phy_hdr->HeaderFormat == 1)
  {
    // LOG_INF("PCC received, type 1, Long Header");
    memcpy(rx_phy_hdr_2->plcf, &evt->hdr, 10);
    get_plcf_2_rev(rx_phy_hdr_2);
    transmitter_srdid = rx_phy_hdr_2->TransmitterIdentity;
    networkID = rx_phy_hdr_2->ShortNetworkID;
    // LOG_INF("Transmitter ID: %x\n", transmitter_srdid);
  }
  
    // LOG_INF("Transmitter ID: %x\n", transmitter_srdid);
    // LOG_INF("Received header from device ID %d",
    // evt->hdr.hdr_type_1.transmitter_id_hi << 8 | evt->hdr.hdr_type_1.transmitter_id_lo);
}

/* Physical Control Channel CRC error notification. */
static void on_pcc_crc_err(const struct nrf_modem_dect_phy_pcc_crc_failure_event *evt)
{
    LOG_INF("PCC CRC ERROR, rssi_2, %d, crc error count,  %d, continuing", calcRSSI(evt->rssi_2, 0), ++crc_errors);
	LOG_DBG("pcc_crc_err cb time %"PRIu64"", modem_time);
}

/* Physical Data Channel reception notification. */
static void on_pdc(const struct nrf_modem_dect_phy_pdc_event *evt)
{
	/* Received RSSI value is in fixed precision format Q14.1 */
	// LOG_INF("Received data (RSSI: %d.%d): %s",
	// 	(evt->rssi_2 / 2), (evt->rssi_2 & 0b1) * 5, (char *)evt->data);

    LOG_INF("________________________________________________________");
    LOG_INF("Packet received (len=%d)", evt->len);
    // k_msleep(200);
    int32_t rx_rssi = calcRSSI(evt->rssi_2, 1);
    // LOG_INF("pdc_cb rssi_2 %d, len %d", rx_rssi, evt->len);

    /* Send data to the queue */
    // if(IS_GATEWAY)
    // {
    //     if (!app_rx_buffer_write(&evt->data[7], 693)) {
    //         LOG_WRN("app_rx_msgq full, dropping app RX packet");
    //     }
    // }
    // else
    // {
    //     rx_fifo_put(evt->data, evt->len, transmitter_srdid, networkID);
    // }
    LOG_WRN("transmitter_srdid: %x", transmitter_srdid);
    LOG_WRN("networkID: %x", networkID);
    rx_fifo_put(evt->data, evt->len, transmitter_srdid, networkID);
    

    // if(!setContinuousRx)
    // {
    //     LOG_INF("Single shot RX complete, cancelling RX handle %d", rxHandle);
    //     nrf_modem_dect_phy_cancel(rxHandle);
    // }    
      
}

/* Physical Data Channel CRC error notification. */
static void on_pdc_crc_err(const struct nrf_modem_dect_phy_pdc_crc_failure_event *evt)
{
    LOG_INF("PDC CRC ERROR, rssi_2, %d, crc error count,  %d, continuing", calcRSSI(evt->rssi_2, 0), ++crc_errors);
	LOG_DBG("pdc_crc_err cb time %"PRIu64"", modem_time);
}

/* RSSI measurement result notification. */
static void on_rssi(const struct nrf_modem_dect_phy_rssi_event *evt)
{
	LOG_DBG("rssi cb time %"PRIu64" carrier %d", modem_time, evt->carrier);
}

static void on_stf_cover_seq_control(const struct nrf_modem_dect_phy_stf_control_event *evt)
{
	LOG_WRN("Unexpectedly in %s\n", (__func__));
}

static void dect_phy_event_handler(const struct nrf_modem_dect_phy_event *evt)
{
	modem_time = evt->time;

	switch (evt->id) {
	case NRF_MODEM_DECT_PHY_EVT_INIT:
		on_init(&evt->init);
		break;
	case NRF_MODEM_DECT_PHY_EVT_DEINIT:
		on_deinit(&evt->deinit);
		break;
	case NRF_MODEM_DECT_PHY_EVT_ACTIVATE:
		on_activate(&evt->activate);
		break;
	case NRF_MODEM_DECT_PHY_EVT_DEACTIVATE:
		on_deactivate(&evt->deactivate);
		break;
	case NRF_MODEM_DECT_PHY_EVT_CONFIGURE:
		on_configure(&evt->configure);
		break;
	case NRF_MODEM_DECT_PHY_EVT_RADIO_CONFIG:
		on_radio_config(&evt->radio_config);
		break;
	case NRF_MODEM_DECT_PHY_EVT_COMPLETED:
		on_op_complete(&evt->op_complete);
		break;
	case NRF_MODEM_DECT_PHY_EVT_CANCELED:
		on_cancel(&evt->cancel);
		break;
	case NRF_MODEM_DECT_PHY_EVT_RSSI:
		on_rssi(&evt->rssi);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PCC:
		on_pcc(&evt->pcc);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PCC_ERROR:
		on_pcc_crc_err(&evt->pcc_crc_err);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PDC:
		on_pdc(&evt->pdc);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PDC_ERROR:
		on_pdc_crc_err(&evt->pdc_crc_err);
		break;
	case NRF_MODEM_DECT_PHY_EVT_TIME:
		on_time_get(&evt->time_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_CAPABILITY:
		on_capability_get(&evt->capability_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_BANDS:
		on_bands_get(&evt->band_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_LATENCY:
		on_latency_info_get(&evt->latency_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_LINK_CONFIG:
		on_link_config(&evt->link_config);
		break;
	case NRF_MODEM_DECT_PHY_EVT_STF_CONFIG:
		on_stf_cover_seq_control(&evt->stf_cover_seq_control);
		break;
	}
}


void modem_init(void)
{
    int err;

    // printk("modem_init: before nrf_modem_lib_init()\n");
    err = nrf_modem_lib_init();
    // printk("modem_init: after nrf_modem_lib_init(), err=%d\n", err);

    // err = nrf_modem_lib_init();
    if (err != 0) {
        LOG_ERR("nrf_modem_lib_init fail %d", err);
        return;
    }

    /* register local event handler */
    err = nrf_modem_dect_phy_event_handler_set(dect_phy_event_handler);
    if (err != 0) {
        LOG_ERR("nrf_modem_dect_phy_event_handler_set failed %d", err);
        return;
    }

    /* init PHY */
    err = nrf_modem_dect_phy_init();
    if (err != 0) {
        LOG_ERR("nrf_modem_dect_phy_init failed %d", err);
        return;
    }

    /* wait for on_init callback to give the semaphore */
    k_sem_take(&modem, K_FOREVER);

    /* You may want to check a fatal flag set by the callback */
    if (modem_fatal_error_flag) {
        LOG_ERR("modem reported fatal error during init");
        return;
    }

    const struct nrf_modem_dect_phy_config_params modem_config = {
        .band_group_index = ((CARRIER >= 525 && CARRIER <= 551)) ? 1 : 0,
        .harq_rx_process_count = 4,
        .harq_rx_expiry_time_us = 5000000
    };

    err = nrf_modem_dect_phy_configure(&modem_config);
    if (err != 0) {
        LOG_ERR("nrf_modem_dect_phy_configure failed %d", err);
        return;
    }

    k_sem_take(&modem, K_FOREVER);
    if (modem_fatal_error_flag) {
        LOG_ERR("modem reported fatal error during configure");
        return;
    }

    const enum nrf_modem_dect_phy_radio_mode modem_mode = NRF_MODEM_DECT_PHY_RADIO_MODE_LOW_LATENCY;
    err = nrf_modem_dect_phy_activate(modem_mode);
    if (err != 0) {
        LOG_ERR("nrf_modem_dect_phy_activate failed %d", err);
        return;
    }

    k_sem_take(&modem, K_FOREVER);
    if (modem_fatal_error_flag) {
        LOG_ERR("modem reported fatal error during activate");
        return;
    }

    config_default_rx_params();

    modem_op_complete();

    LOG_INF("Carrier: %d\n", CARRIER);
    LOG_INF("Environment checked and working\n");
    LOG_INF("----------------------------------------------------------\n");
}


void modem_rx(uint32_t rxMode, int time_ms)
{
    modem_op_start();

    if(rxMode == NRF_MODEM_DECT_PHY_RX_MODE_CONTINUOUS)
    {
        setContinuousRx = true;
        // LOG_INF("Starting CONTINUOUS RX for %d ms", time_ms);
    }
    else
    {
        setContinuousRx = false;
        // LOG_INF("Starting SINGLE SHOT RX for %d ms", time_ms);
    }
    
    rxOpsParams.duration = time_ms * NRF_MODEM_DECT_MODEM_TIME_TICK_RATE_KHZ;
    rxOpsParams.mode = rxMode;

    LOG_INF(",");

    int ret = nrf_modem_dect_phy_rx(&rxOpsParams);
    if (ret != 0) {
        LOG_INF("RX FAIL %d", ret);
    }
    
}

void modem_tx(uint8_t* data, size_t datasize, uint8_t* phyheader, size_t phyheadersize, int scheduled)
{
    modem_op_start();

  memcpy(_txData, data, datasize);

//   if(phyheadersize==5) LOG_INF("TX with PHY header type 1");
//   if(phyheadersize==10) LOG_INF("TX with PHY header type 2");

  if(phyheadersize==5)memcpy(&phyHeader.type_1, phyheader, phyheadersize);
  else memcpy(&phyHeader.type_2, phyheader, phyheadersize);

//   LOG_INF("TX scheduled in %d ms", scheduled);

//   LOG_INF("Time: %d", modem_time);
  struct nrf_modem_dect_phy_tx_params txOpsParams;
  //immediate operation
  if(scheduled==0) txOpsParams.start_time = 0;
  else  txOpsParams.start_time = (scheduled * NRF_MODEM_DECT_MODEM_TIME_TICK_RATE_KHZ) + modem_time;
    // txOpsParams.start_time = scheduled;
  txOpsParams.handle = txHandle;
  //neteork id value, used in rx filtering
  txOpsParams.network_id = 0x0a;
  if(phyheadersize==5)txOpsParams.phy_type = 0;
  else txOpsParams.phy_type = 1;
  txOpsParams.lbt_rssi_threshold_max = 0;
  //  EU carrier, see ETSI TS 103 636-2 5.4.2 for the calculation 
  txOpsParams.carrier = CARRIER;
  //NOTE !!! no LBT done
  txOpsParams.lbt_period = 0 * 10 * NRF_MODEM_DECT_MODEM_TIME_TICK_RATE_KHZ;
  txOpsParams.phy_header = &phyHeader;
  txOpsParams.data = _txData;
  txOpsParams.data_size = datasize;

  int err=nrf_modem_dect_phy_tx(&txOpsParams);
  if(err!=0) LOG_WRN("TX FAIL %d", err);
  k_sem_take(&modem, K_FOREVER);
  
  if(txHandle==30000)txHandle=1;
  else txHandle++;
  LOG_INF(".");


}

void request_rssi_measurement(void)
{
    /* Example: fill rxOpsParams for an RSSI measurement and call modem_rx or a specific API */
    LOG_DBG("request_rssi_measurement: requesting RSSI measurement");
    /* In your main you used struct nrf_modem_dect_phy_rssi_params and request_rssi_measurement(rssiParams) */
} 

void config_default_rx_params(void)
{
    // Clear structure before configuring
    memset(&rxOpsParams, 0, sizeof(rxOpsParams));

    rxOpsParams.start_time = 0; //start immediately
    rxOpsParams.handle = 31400;
    rxOpsParams.network_id = 0;  // Use default value or NETWORK_ID if available
    rxOpsParams.mode = NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT;
    rxOpsParams.link_id = NRF_MODEM_DECT_PHY_LINK_UNSPECIFIED;
    rxOpsParams.rssi_level = -60;
    rxOpsParams.carrier = CARRIER;
    // modem clock ticks NRF_MODEM_DECT_MODEM_TIME_TICK_RATE_KHZ --> 69120*1000* TIME_S
    rxOpsParams.duration = 2*69120*1000; 
    //filter on the short network id, last 8 bits of the network identifier in dect nr
    rxOpsParams.filter.short_network_id = (uint8_t)(0x0a);  // Default value
    rxOpsParams.filter.is_short_network_id_used = 0;
    //listen for everything (broadcast mode used)
    rxOpsParams.filter.receiver_identity = 0;
}

void disable_rx_filter(void)
{
    // LOG_INF("RX filter disabled");
    rxOpsParams.filter.is_short_network_id_used = 0;
    rxOpsParams.filter.short_network_id = 0;
    rxOpsParams.filter.receiver_identity = 0;

    return;
}

void enable_rx_filter(uint8_t short_network_id, uint16_t receiver_identity)
{
  if (short_network_id != 0)
  {
    rxOpsParams.filter.is_short_network_id_used = 1;
    rxOpsParams.filter.short_network_id = short_network_id;
    LOG_INF("RX filter enabled: ShortNetworkID %x", short_network_id);
  }
  else
  {
    rxOpsParams.filter.is_short_network_id_used = 0;
    rxOpsParams.filter.short_network_id = 0;
    LOG_INF("RX filter disabled: ShortNetworkID");
  }
  
  if (receiver_identity != 0)
  {
    rxOpsParams.filter.receiver_identity = receiver_identity;
    LOG_INF("RX filter enabled: ReceiverID %x", receiver_identity);
  }

  return;
}

void set_TXParams(struct TXParams new_tp)
{
    tp = new_tp;
}

void set_RXParams(struct nrf_modem_dect_phy_rx_params new_rp)
{
    rxOpsParams = new_rp;
}

int get_txHandle(void)
{
    return txHandle;
}

int get_rxHandle(void)
{
    return rxHandle;
}

void modem_op_start(void)
{
    modem_free = false;
    /* Notify tx_consumer that modem is busy */
    modem_operator_set_modem_free(false);
    // LOG_INF("Modem busy");
} 

void modem_op_complete(void)
{
    modem_free = true;
    /* Notify tx_consumer that modem is free */
    modem_operator_set_modem_free(true);
    // LOG_INF("Modem free");
}

bool is_modem_free(void)
{
    return modem_free;
}

uint16_t get_transmitter_srdid(void)
{
    return transmitter_srdid;
}