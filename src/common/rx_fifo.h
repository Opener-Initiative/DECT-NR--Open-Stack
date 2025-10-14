#ifndef RX_FIFO_H
#define RX_FIFO_H

#include <zephyr/kernel.h>
#include <stdint.h>

#define RX_BUFFER_SIZE 256

struct rx_packet {
    void *fifo_reserved;
    uint8_t data[RX_BUFFER_SIZE];
    uint32_t len;
};

void rx_fifo_put(const void *data, uint32_t len);
struct rx_packet *rx_fifo_get(k_timeout_t timeout);
void rx_fifo_free(struct rx_packet *pkt);

#endif // RX_FIFO_H
