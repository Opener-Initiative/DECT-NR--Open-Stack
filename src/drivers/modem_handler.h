#ifndef MODEM_HANDLER_H
#define MODEM_HANDLER_H
#include <stddef.h>
#include <stdint.h>
void modem_init(void);
void modem_rx(uint32_t rxMode, int time_s);
void modem_tx(uint8_t* data, size_t datasize, uint8_t* phyheader, size_t phyheadersize);
void request_rssi_measurement(void);
void disable_rx_filter(void);
void config_default_rx_params(void);
void enable_rx_filter(uint8_t network_id, uint16_t receiver_id);
#endif
