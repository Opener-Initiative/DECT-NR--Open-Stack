#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define APP_TX_BUFFER_SIZE 693
#define APP_RX_BUFFER_SIZE 693

bool app_tx_buffer_write(const uint8_t *data, size_t len);
int app_tx_buffer_read(uint8_t *out, size_t maxlen);

bool app_rx_buffer_write(const uint8_t *data, size_t len);
int app_rx_buffer_read(uint8_t *out, size_t maxlen);

bool app_tx_buffer_available(void);
bool app_rx_buffer_available(void);
