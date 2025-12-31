#ifndef TX_FIFO_H
#define TX_FIFO_H

#include <zephyr/kernel.h>
#include <stdint.h>

#define TX_BUFFER_SIZE 700
#define TX_HEADER_SIZE 10

struct tx_packet {
    void *fifo_reserved;
    uint8_t data[TX_BUFFER_SIZE];
    uint32_t len;
    uint16_t transmitter_srdid;
    uint32_t scheduled;
    uint8_t header_data[TX_HEADER_SIZE];
    uint32_t header_len;
};

void tx_fifo_pool_init(void);
// void tx_fifo_put(const void *data, uint32_t len, uint16_t transmitter_srdid);
void tx_fifo_put(const void *data, uint32_t len, const void *header, 
    uint32_t header_len, uint32_t scheduled);
struct tx_packet *tx_fifo_get(k_timeout_t timeout);
void tx_fifo_free(struct tx_packet *pkt);
int tx_fifo_count(void);
int tx_fifo_dropped_count(void);
int tx_pool_exhausted_count(void);

#endif // TX_FIFO_H
