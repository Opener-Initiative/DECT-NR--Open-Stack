#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include "rx_fifo.h"
#include <zephyr/sys/atomic.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(rx_fifo);

/// @brief Global FIFO queue for received packets
K_FIFO_DEFINE(rx_fifo);

/* Static packet pool to avoid dynamic malloc which fragments the heap */
#ifndef RX_PACKET_POOL_SIZE
#define RX_PACKET_POOL_SIZE 32
#endif

static struct rx_packet packet_pool[RX_PACKET_POOL_SIZE];
static atomic_t packet_available[RX_PACKET_POOL_SIZE];  // 0=busy, 1=available
static K_FIFO_DEFINE(packet_fifo);  // FIFO of available packets

static int pkg_count = 0;

/* Atomic counter of elements in the FIFO (monitoring only) */
static atomic_t rx_fifo_len = ATOMIC_INIT(0);
/* Number of packets dropped due to FIFO overflow */
static atomic_t rx_fifo_dropped = ATOMIC_INIT(0);
/* Number of attempts to get a packet when the pool was empty */
static atomic_t pool_exhausted = ATOMIC_INIT(0);
/* Maximum number of packets allowed in the FIFO before dropping starts */
#ifndef RX_FIFO_MAX_ITEMS
#define RX_FIFO_MAX_ITEMS 64
#endif

/* Initialize the pool at startup */
void rx_fifo_pool_init(void)
{
    for (int i = 0; i < RX_PACKET_POOL_SIZE; i++) {
        atomic_set(&packet_available[i], 1);  // Mark as available
        k_fifo_put(&packet_fifo, &packet_pool[i]);
    }
    LOG_INF("rx_fifo pool initialized with %d packets", RX_PACKET_POOL_SIZE);
}

/// @brief Put a packet into the FIFO
/// @param data 
/// @param len 
void rx_fifo_put(const void *data, uint32_t len, uint16_t transmitter_srdid, uint8_t networkID)
{
    // If FIFO is beyond threshold, drop packet to avoid unbounded memory growth
    if (atomic_get(&rx_fifo_len) >= RX_FIFO_MAX_ITEMS) {
        atomic_inc(&rx_fifo_dropped);
        LOG_WRN("rx_fifo full (>%d), dropping packet (dropped=%d)", RX_FIFO_MAX_ITEMS, (int)atomic_get(&rx_fifo_dropped));
        return;
    }

    // Get a packet from the pool (non-blocking)
    struct rx_packet *pkt = k_fifo_get(&packet_fifo, K_NO_WAIT);
    if (!pkt) {
        atomic_inc(&pool_exhausted);
        LOG_WRN("packet pool exhausted (attempts=%d), dropping packet", (int)atomic_get(&pool_exhausted));
        return;
    }

    pkt->len = MIN(len, RX_BUFFER_SIZE);
    memcpy(pkt->data, data, pkt->len);
    pkt->transmitter_srdid = transmitter_srdid;
    pkt->networkID = networkID;

    k_fifo_put(&rx_fifo, pkt);
    atomic_inc(&rx_fifo_len);
    pkg_count++;
    LOG_INF("Packet enqueued, length =%d", len);
    // LOG_INF("Packet enqueued, length =%d", pkt->len);
}

/// @brief Extract a packet from the FIFO, waiting up to 'timeout'
/// @param timeout Maximum time to wait (K_NO_WAIT, K_FOREVER, or k_timeout_t)
/// @return Pointer to the extracted packet, or NULL if timeout occurred

struct rx_packet *rx_fifo_get(k_timeout_t timeout)
{
    // LOG_INF("rx_fifo_get called\n");
    struct rx_packet *pkt = k_fifo_get(&rx_fifo, timeout);
    if (pkt) {
        /* element has already been removed from the FIFO */
        atomic_dec(&rx_fifo_len);
        pkg_count--;
    }
    return pkt;
}

/* Release the packet back to the pool after processing */
void rx_fifo_free(struct rx_packet *pkt)
{
    if (pkt) {
        // Return the packet to the pool
        k_fifo_put(&packet_fifo, pkt);
    }
}

int rx_fifo_dropped_count(void)
{
    return (int)atomic_get(&rx_fifo_dropped);
}

int rx_pool_exhausted_count(void)
{
    return (int)atomic_get(&pool_exhausted);
}

/* Lightweight monitor thread to log pool/fifo stats periodically */
static void rx_fifo_monitor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    while (1) {
        int dropped = rx_fifo_dropped_count();
        int exhausted = rx_pool_exhausted_count();
        int in_fifo = rx_fifo_count();
        if (dropped || exhausted || in_fifo) {
            LOG_WRN("rx_fifo stats: in_fifo=%d dropped=%d pool_exhausted=%d", in_fifo, dropped, exhausted);
        } else {
            LOG_DBG("rx_fifo stats: in_fifo=%d", in_fifo);
        }
        k_sleep(K_SECONDS(5));
    }
}

K_THREAD_DEFINE(rx_fifo_monitor_tid, 1024, rx_fifo_monitor_thread, NULL, NULL, NULL, 5, 0, 0);

int rx_fifo_count(void)
{
    // LOG_INF("rx_fifo_count: %d", (int)atomic_get(&rx_fifo_len));
    // LOG_INF("pkg_count: %d", pkg_count);

    return pkg_count;
}
