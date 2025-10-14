#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include "rx_fifo.h"

K_FIFO_DEFINE(rx_fifo);

/// @brief Add a received packet to the FIFO
/// @param data Pointer to the packet data
void rx_fifo_put(const void *data, uint32_t len)
{
    struct rx_packet *pkt = k_malloc(sizeof(struct rx_packet));
    if (!pkt) {
        printk("ERROR: No memory for rx_packet\n");
        return;
    }

    pkt->len = MIN(len, RX_BUFFER_SIZE);
    memcpy(pkt->data, data, pkt->len);

    k_fifo_put(&rx_fifo, pkt);
}

/// @brief Extract a packet from the FIFO, waiting up to 'timeout'
/// @param timeout Maximum time to wait (K_NO_WAIT, K_FOREVER, or k_timeout_t)
/// @return Pointer to the extracted packet, or NULL if timeout occurred
struct rx_packet *rx_fifo_get(k_timeout_t timeout)
{
    return k_fifo_get(&rx_fifo, timeout);
}

/* Libera la memoria del paquete después de procesarlo */
void rx_fifo_free(struct rx_packet *pkt)
{
    if (pkt) {
        k_free(pkt);
    }
}
