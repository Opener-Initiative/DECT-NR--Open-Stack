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
#include "../app/app_state.h"

LOG_MODULE_REGISTER(modem_h);

K_SEM_DEFINE(modem, 1, 1);
static int rxHandle = 31400;
static int txHandle = 1;
struct nrf_modem_dect_phy_rx_params rxOpsParams = {0};
union nrf_modem_dect_phy_hdr phyHeader;
static uint8_t _txData[DATA_LEN];
static uint8_t _rxData[DATA_LEN];

static struct TXParams tp;

static int crc_errors = 0;
static float rssi_average = 0;
static int n = 0;
static int8_t rssi_measure = 0;
plcf_10_t rx_phy_hdr[5];
plcf_20_t rx_phy_hdr_2[10];
uint16_t transmitter_srdid;

struct nrf_modem_dect_phy_rssi_params rssiParams = {
  .start_time = 0,
  .handle = 4,
  .carrier = CARRIER,
  .duration = 24,
  .reporting_interval = NRF_MODEM_DECT_PHY_RSSI_INTERVAL_24_SLOTS,
};

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

void init(const uint64_t *time, int16_t temp, enum nrf_modem_dect_phy_err err, const struct nrf_modem_dect_phy_modem_cfg *cfg)
{
  if(err==0) {
    // LOG_INF("DECT Init done, temperature %d", temp);
  }
  else {
    LOG_ERR("INIT FAILED");
    LOG_INF("Init failed, Exit\n");
  }
  k_sem_give(&modem);
}

void op_complete(const uint64_t *time, int16_t temperature, enum nrf_modem_dect_phy_err err, uint32_t handle)
{
  LOG_DBG("operation_complete_cb Status %d, Temp %d, Handle %d", err, temperature, handle);
  k_sem_give(&modem);
  return;
}

//// stf_cover_seq_control callback implemented as empty for newer versions
void stf_cover_seq_control(const uint64_t *time, int err) {
  LOG_DBG("stf_cover_seq_control callback called with err %d", err);
}

void rssi(const uint64_t *time, const struct nrf_modem_dect_phy_rssi_meas *status)
{   
  int8_t measure = *(status->meas);
  printk("%d: %+d\n", status->carrier, measure);
  // LOG_INF("RSSI Measurement: %+d dBm\n", (int8_t)status->meas);
  // measured_rssi = status->meas;
  // LOG_INF("RSSI meas length: %d\n", status->meas_len);

  rssi_measure = measure;

  k_sem_give(&modem);
  return;
}

void pdc_crc_err_cb(const uint64_t *time, const struct nrf_modem_dect_phy_rx_pdc_crc_failure *crc_failure)
{
    crc_errors++;
    int16_t resp = calcRSSI(crc_failure->rssi_2, 0);
    LOG_INF("PDC CRC ERROR, rssi_2 %d, crc error count %d", resp, crc_errors);
}

void pdc_crc_err(
  const uint64_t *time, const struct nrf_modem_dect_phy_rx_pdc_crc_failure *crc_failure)
{
  crc_errors++;
  int16_t resp=calcRSSI(crc_failure->rssi_2, 0);
  LOG_INF("PDC CRC ERROR, rssi_2, %d, crc error count, %d, continuing", resp, crc_errors);
}

void link_config(const uint64_t *time, enum nrf_modem_dect_phy_err err)
{
  return;
}

void time_get(const uint64_t *time, enum nrf_modem_dect_phy_err err)
{
  LOG_DBG("Time query response time %"PRIu64" Status %d", *time, err); 
}

void capability_get(const uint64_t *time, enum nrf_modem_dect_phy_err err,const struct nrf_modem_dect_phy_capability *capability)
{
  LOG_DBG("Capability query response FIXME %"PRIu64" Status %d", *time, err); 
}

void deinit(const uint64_t *time, enum nrf_modem_dect_phy_err err)
{
  LOG_DBG("DEINIT response time %"PRIu64" Status %d", *time, err); 
}

void rx_stop(const uint64_t *time, enum nrf_modem_dect_phy_err err, uint32_t handle)
{
  LOG_DBG("operation_stop_cb Status %d Handle %d", err, handle);
  k_sem_give(&modem);
  return;
}

void pcc(
  const uint64_t *time,
  const struct nrf_modem_dect_phy_rx_pcc_status *status,
  const union nrf_modem_dect_phy_hdr *hdr)
{
  LOG_DBG("pcc_cb phy_header_valid %d rssi_2 %d", status->header_status, status->rssi_2);
  
  memcpy(rx_phy_hdr->plcf, &hdr->type_1, 5);
  // printk("Net ID: %x\n", hdr->type_1[1]);

  get_plcf_1_rev(rx_phy_hdr);
  // printk("ShortNetworkID: %x\n", rx_phy_hdr->ShortNetworkID);
  
  if(rx_phy_hdr->HeaderFormat == 0){
    // LOG_INF("PCC received, type 0, Short Header");
    transmitter_srdid = rx_phy_hdr->TransmitterIdentity;
    // LOG_INF("Transmitter ID: %x\n", transmitter_srdid);
  }
  else if(rx_phy_hdr->HeaderFormat == 1)
  {
    // LOG_INF("PCC received, type 1, Long Header");
    memcpy(rx_phy_hdr_2->plcf, &hdr->type_2, 10);
    get_plcf_2_rev(rx_phy_hdr_2);
    transmitter_srdid = rx_phy_hdr_2->TransmitterIdentity;
    // LOG_INF("Transmitter ID: %x\n", transmitter_srdid);
  }

  return;
}

void pcc_crc_err(const uint64_t *time, const struct nrf_modem_dect_phy_rx_pcc_crc_failure *crc_failure)
{
  crc_errors++;
  int16_t resp=calcRSSI(crc_failure->rssi_2, 0);
  LOG_INF("PCC CRC ERROR, rssi_2, %d, crc error count,  %d, continuing", resp, crc_errors);
}


void pdc(const uint64_t *time,
         const struct nrf_modem_dect_phy_rx_pdc_status *status,
         const void *data, uint32_t len)
{
    LOG_INF("Packet received (len=%d)", len);
    int32_t rx_rssi = calcRSSI(status->rssi_2, 1);
    // LOG_INF("pdc_cb rssi_2 %d, len %d", rx_rssi, len);

    //// Queue data for later processing depending on app state
    rx_fifo_put(data, len);
}



/* config struct exportado a la API */
struct nrf_modem_dect_phy_callbacks dect_cb_config = {
    .init = init,
    .deinit = deinit,
    .op_complete = op_complete,
    .rx_stop = rx_stop,
    .pcc = pcc,
    .pcc_crc_err = pcc_crc_err,
    .pdc = pdc,
    .pdc_crc_err = pdc_crc_err,
    .rssi = rssi,
    .link_config = link_config,
    .time_get = time_get,
    .capability_get = capability_get,
    #if defined(BOARD_USED) && BOARD_USED == 9151
        .stf_cover_seq_control = stf_cover_seq_control, 
    #endif
};

//new parameters for HARQ operation, not used in this sample
const struct nrf_modem_dect_phy_init_params init_params ={
	.harq_rx_expiry_time_us=5000000,
	.harq_rx_process_count=1
};



void modem_init(void)
{
    /* Inicializa nrf_modem_lib si hace falta */
    int err = nrf_modem_lib_init();
    if (err != 0) {
        LOG_ERR("nrf_modem_lib_init fail %d", err);
    }

    err=nrf_modem_dect_phy_callback_set(&dect_cb_config);
    if(err!=0) {
        LOG_INF("ERROR settings callbacks %d\n",err);
    }
    err=nrf_modem_dect_phy_init(&init_params);
    if(err!=0) {
        LOG_INF("ERROR initializing modem PHY %d\n",err);
        return;
    }

  config_default_rx_params();

    
  LOG_INF("Carrier: %d\n", CARRIER);
  LOG_INF("Environment checked and working\n");
  LOG_INF("----------------------------------------------------------\n");
}

void modem_rx(uint32_t rxMode, int time_s)
{

    k_sem_take(&modem, K_FOREVER);
    int ret = nrf_modem_dect_phy_rx(&rxOpsParams);
    if (ret != 0) {
        LOG_ERR("RX FAIL %d", ret);
    }
}

void modem_tx(uint8_t* data, size_t datasize, uint8_t* phyheader, size_t phyheadersize)
{
  memcpy(_txData, data, datasize);

  if(phyheadersize==5)memcpy(&phyHeader.type_1, phyheader, phyheadersize);
  else memcpy(&phyHeader.type_2, phyheader, phyheadersize);

  struct nrf_modem_dect_phy_tx_params txOpsParams;
  //immediate operation
  txOpsParams.start_time = 0; 
  txOpsParams.handle = txHandle;
  //neteork id value, used in rx filtering
  txOpsParams.network_id = 0x0a;
  if(phyheadersize==5)txOpsParams.phy_type = 0;
  else txOpsParams.phy_type = 1;
  txOpsParams.lbt_rssi_threshold_max = 0;
  //  EU carrier, see ETSI TS 103 636-2 5.4.2 for the calculation 
  txOpsParams.carrier = CARRIER;
  //NOTE !!! no LBT done
  txOpsParams.lbt_period = 0;
  txOpsParams.phy_header = &phyHeader;
  txOpsParams.data = _txData;
  txOpsParams.data_size = datasize;

  k_sem_take(&modem, K_FOREVER);
  int err=nrf_modem_dect_phy_tx(&txOpsParams);
  if(err!=0) LOG_ERR("TX FAIL %d", err);
  if(txHandle==30000)txHandle=1;
  else txHandle++;
  LOG_INF(".\n");


}

void request_rssi_measurement(void)
{
    int err = nrf_modem_dect_phy_rssi(&rssiParams);
    if (err != 0) {
        LOG_INF("RSSI measurement request failed with error %d\n", err);
    } else {
        // printk("RSSI measurement request succeeded.\n");
    }
}

void config_default_rx_params(void)
{
    memset(&rxOpsParams, 0, sizeof(rxOpsParams));

    rxOpsParams.start_time = 0; //start immediately
    rxOpsParams.handle = 31400;
    rxOpsParams.network_id = 0;  // Usar valor por defecto o NETWORK_ID si está disponible
    rxOpsParams.mode = NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT;
    rxOpsParams.link_id = NRF_MODEM_DECT_PHY_LINK_UNSPECIFIED;
    rxOpsParams.rssi_level = -60;
    rxOpsParams.carrier = CARRIER;
    // modem clock ticks NRF_MODEM_DECT_MODEM_TIME_TICK_RATE_KHZ --> 69120*1000* TIME_S
    rxOpsParams.duration = 2*69120*1000; 
    //filter on the short network id, last 8 bits of the network identifier in dect nr
    rxOpsParams.filter.short_network_id = (uint8_t)(0x0a);
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
