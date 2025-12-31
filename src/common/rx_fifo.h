#ifndef RX_FIFO_H
#define RX_FIFO_H

#include <zephyr/kernel.h>
#include <stdint.h>

#define RX_BUFFER_SIZE 700

struct rx_packet {
    void *fifo_reserved;
    uint8_t data[RX_BUFFER_SIZE];
    uint32_t len;
    uint16_t transmitter_srdid;
    uint8_t networkID;
};

void rx_fifo_pool_init(void);
void rx_fifo_put(const void *data, uint32_t len, uint16_t transmitter_srdid, uint8_t networkID);
struct rx_packet *rx_fifo_get(k_timeout_t timeout);
void rx_fifo_free(struct rx_packet *pkt);
int rx_fifo_count(void);
int rx_fifo_dropped_count(void);
int rx_pool_exhausted_count(void);

#endif // RX_FIFO_H
