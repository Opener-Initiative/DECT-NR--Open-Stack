#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include "tx_fifo.h"
#include <zephyr/sys/atomic.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(tx_fifo);

/// @brief Global FIFO queue for transmitted packets
K_FIFO_DEFINE(tx_fifo);

/* Static packet pool to avoid dynamic malloc which fragments the heap */
#ifndef TX_PACKET_POOL_SIZE
#define TX_PACKET_POOL_SIZE 32
#endif

static struct tx_packet packet_pool[TX_PACKET_POOL_SIZE];
static atomic_t packet_available[TX_PACKET_POOL_SIZE];  // 0=busy, 1=available
static K_FIFO_DEFINE(packet_fifo);  // FIFO of available packets

static int pkg_count = 0;

/* Atomic counter of elements in the FIFO (monitoring only) */
static atomic_t tx_fifo_len = ATOMIC_INIT(0);
/* Number of packets dropped due to FIFO overflow */
static atomic_t tx_fifo_dropped = ATOMIC_INIT(0);
/* Number of attempts to get a packet when the pool was empty */
static atomic_t pool_exhausted = ATOMIC_INIT(0);
/* Maximum number of packets allowed in the FIFO before dropping starts */
#ifndef TX_FIFO_MAX_ITEMS
#define TX_FIFO_MAX_ITEMS 64
#endif

/* Initialize the pool at startup */
void tx_fifo_pool_init(void)
{
    for (int i = 0; i < TX_PACKET_POOL_SIZE; i++) {
        atomic_set(&packet_available[i], 1);  // Mark as available
        k_fifo_put(&packet_fifo, &packet_pool[i]);
    }
    LOG_INF("tx_fifo pool initialized with %d packets", TX_PACKET_POOL_SIZE);
}

// /// @brief Put a packet into the FIFO
// /// @param data 
// /// @param len 
// void tx_fifo_put(const void *data, uint32_t len, uint16_t transmitter_srdid)
// {
//     // If FIFO is beyond threshold, drop packet to avoid unbounded memory growth
//     if (atomic_get(&tx_fifo_len) >= TX_FIFO_MAX_ITEMS) {
//         atomic_inc(&tx_fifo_dropped);
//         LOG_WRN("tx_fifo full (>%d), dropping packet (dropped=%d)", TX_FIFO_MAX_ITEMS, (int)atomic_get(&tx_fifo_dropped));
//         return;
//     }

//     // Get a packet from the pool (non-blocking)
//     struct tx_packet *pkt = k_fifo_get(&packet_fifo, K_NO_WAIT);
//     if (!pkt) {
//         atomic_inc(&pool_exhausted);
//         LOG_WRN("packet pool exhausted (attempts=%d), dropping packet", (int)atomic_get(&pool_exhausted));
//         return;
//     }

//     pkt->len = MIN(len, TX_BUFFER_SIZE);
//     memcpy(pkt->data, data, pkt->len);
//     pkt->transmitter_srdid = transmitter_srdid;
//     pkt->header_len = 0;  /* No header by default */

//     k_fifo_put(&tx_fifo, pkt);
//     atomic_inc(&tx_fifo_len);
//     pkg_count++;
// }

/// @brief Extract a packet from the FIFO, waiting up to 'timeout'
/// @param timeout Maximum time to wait (K_NO_WAIT, K_FOREVER, or k_timeout_t)
/// @return Pointer to the extracted packet, or NULL if timeout occurred

struct tx_packet *tx_fifo_get(k_timeout_t timeout)
{
    // LOG_INF("tx_fifo_get called\n");
    struct tx_packet *pkt = k_fifo_get(&tx_fifo, timeout);
    if (pkt) {
        /* element has already been removed from the FIFO */
        atomic_dec(&tx_fifo_len);
        pkg_count--;
    }
    else
    {
        // LOG_INF("tx_fifo_get: timeout occurred\n");
    }
    return pkt;
}

/// @brief Put a packet with PHY header into the FIFO
/// @param data Packet payload
/// @param len Payload length
/// @param header PHY header data
/// @param header_len PHY header length (max TX_HEADER_SIZE)
/// @param scheduled Time until scheduled TX
void tx_fifo_put(const void *data, uint32_t len, const void *header, 
                uint32_t header_len, uint32_t scheduled)
{
    // If FIFO is beyond threshold, drop packet to avoid unbounded memory growth
    if (atomic_get(&tx_fifo_len) >= TX_FIFO_MAX_ITEMS) {
        atomic_inc(&tx_fifo_dropped);
        LOG_WRN("tx_fifo full (>%d), dropping packet (dropped=%d)", TX_FIFO_MAX_ITEMS, (int)atomic_get(&tx_fifo_dropped));
        return;
    }

    // Get a packet from the pool (non-blocking)
    struct tx_packet *pkt = k_fifo_get(&packet_fifo, K_NO_WAIT);
    if (!pkt) {
        atomic_inc(&pool_exhausted);
        LOG_WRN("packet pool exhausted (attempts=%d), dropping packet", (int)atomic_get(&pool_exhausted));
        return;
    }

    pkt->len = MIN(len, TX_BUFFER_SIZE);
    memcpy(pkt->data, data, pkt->len);
    pkt->scheduled = scheduled;
    
    /* Copy header if provided */
    if (header && header_len > 0) {
        pkt->header_len = MIN(header_len, TX_HEADER_SIZE);
        memcpy(pkt->header_data, header, pkt->header_len);
    } else {
        pkt->header_len = 0;
    }

    k_fifo_put(&tx_fifo, pkt);
    atomic_inc(&tx_fifo_len);
    pkg_count++;
    // LOG_WRN("tx_fifo_put: pkg_count=%d", pkg_count);
}

/* Release the packet back to the pool after processing */
void tx_fifo_free(struct tx_packet *pkt)
{
    if (pkt) {
        // Return the packet to the pool
        k_fifo_put(&packet_fifo, pkt);
    }
}

int tx_fifo_dropped_count(void)
{
    return (int)atomic_get(&tx_fifo_dropped);
}

int tx_pool_exhausted_count(void)
{
    return (int)atomic_get(&pool_exhausted);
}

/* Lightweight monitor thread to log pool/fifo stats periodically */
static void tx_fifo_monitor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    while (1) {
        int dropped = tx_fifo_dropped_count();
        int exhausted = tx_pool_exhausted_count();
        int in_fifo = tx_fifo_count();
        if (dropped || exhausted || in_fifo) {
            LOG_WRN("tx_fifo stats: in_fifo=%d dropped=%d pool_exhausted=%d", in_fifo, dropped, exhausted);
        } else {
            LOG_DBG("tx_fifo stats: in_fifo=%d", in_fifo);
        }
        k_sleep(K_SECONDS(5));
    }
}

K_THREAD_DEFINE(tx_fifo_monitor_tid, 1024, tx_fifo_monitor_thread, NULL, NULL, NULL, 5, 0, 0);

int tx_fifo_count(void)
{
    // LOG_INF("tx_fifo_count: %d", (int)atomic_get(&tx_fifo_len));
    // LOG_INF("pkg_count: %d", pkg_count);

    return pkg_count;
}
