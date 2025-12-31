#include "data_buffers.h"
#include <string.h>
#include <zephyr/kernel.h>

static uint8_t tx_buffer[APP_TX_BUFFER_SIZE];
static size_t tx_len = 0;
K_MUTEX_DEFINE(tx_lock);

/* RX path: use a message queue so multiple packets can be queued for the
 * application without overwriting a single shared buffer. This is safer
 * under bursts and from different contexts. */
#ifndef APP_RX_MSGQ_DEPTH
#define APP_RX_MSGQ_DEPTH 4
#endif
K_MSGQ_DEFINE(app_rx_msgq, APP_RX_BUFFER_SIZE, APP_RX_MSGQ_DEPTH, 4);

/// @brief Write data to the TX buffer
/// @param data 
/// @param len 
/// @return true if successful, false if buffer overflow
bool app_tx_buffer_write(const uint8_t *data, size_t len) {
    if (len > APP_TX_BUFFER_SIZE) return false;
    k_mutex_lock(&tx_lock, K_FOREVER);
    memcpy(tx_buffer, data, len);
    tx_len = len;
    k_mutex_unlock(&tx_lock);
    return true;
}

/// @brief Read data from the TX buffer
/// @param out 
/// @param maxlen 
/// @return Number of bytes read, or -1 if no data is available
int app_tx_buffer_read(uint8_t *out, size_t maxlen) {
    k_mutex_lock(&tx_lock, K_FOREVER);
    if (tx_len == 0) {
        k_mutex_unlock(&tx_lock);
        return -1;
    }
    size_t copy_len = (tx_len > maxlen) ? maxlen : tx_len;
    memcpy(out, tx_buffer, copy_len);
    tx_len = 0;
    k_mutex_unlock(&tx_lock);
    return copy_len;
}

/// @brief Check if the TX buffer has data available
bool app_tx_buffer_available(void) {
    bool available;
    k_mutex_lock(&tx_lock, K_FOREVER);
    available = (tx_len > 0);
    k_mutex_unlock(&tx_lock);
    return available;
}

/// @brief Write data to the RX buffer
/// @param data
/// @param len
/// @return true if successful, false if buffer overflow
bool app_rx_buffer_write(const uint8_t *data, size_t len) {
    if (len > APP_RX_BUFFER_SIZE) return false;
    int ret = k_msgq_put(&app_rx_msgq, data, K_NO_WAIT);
    if (ret != 0) {
        return false;
    }
    return true;
}

/// @brief Read data from the RX buffer
/// @param out 
/// @param maxlen 
/// @return Number of bytes read, or -1 if no data is available
int app_rx_buffer_read(uint8_t *out, size_t maxlen) {
    if (maxlen > APP_RX_BUFFER_SIZE) {
        return -1;
    }
    int ret = k_msgq_get(&app_rx_msgq, out, K_NO_WAIT);
    if (ret != 0) return -1;
    return APP_RX_BUFFER_SIZE;
}

/// @brief Check if the RX buffer has data available
bool app_rx_buffer_available(void) {
    return k_msgq_num_used_get(&app_rx_msgq) > 0;
}
