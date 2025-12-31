#include "modem_operator.h"
#include "tx_fifo.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <errno.h>
#include <string.h>
#include "../drivers/modem_handler.h"
#include <zephyr/sys/byteorder.h>

LOG_MODULE_REGISTER(modem_operator);

/* Internal atomic flag that indicates modem availability. Default: true */
static atomic_t modem_free_flag = ATOMIC_INIT(1);

/* RX request structure and queue */
struct rx_request {
    uint32_t mode;
    int time_ms;
};

#define MODEM_OPERATOR_RX_QUEUE_SIZE 8
K_MSGQ_DEFINE(rx_req_q, sizeof(struct rx_request), MODEM_OPERATOR_RX_QUEUE_SIZE, 4);

int modem_operator_request_rx(uint32_t rxMode, int time_ms)
{
    struct rx_request req = {
        .mode = rxMode,
        .time_ms = time_ms,
    };
    int res = k_msgq_put(&rx_req_q, &req, K_NO_WAIT);
    return (res == 0) ? 0 : -ENOMEM;
}

void modem_operator_init(void)
{
    atomic_set(&modem_free_flag, 1);
    /* drain queue if needed (safe init) */
    struct rx_request tmp;
    while (k_msgq_get(&rx_req_q, &tmp, K_NO_WAIT) == 0) { }
}

void modem_operator_set_modem_free(bool free)
{
    atomic_set(&modem_free_flag, free ? 1 : 0);
}

bool modem_operator_is_modem_free(void)
{
    return atomic_get(&modem_free_flag) != 0;
}

/* Consumer thread: read packets from tx_fifo and send them when modem is free */
static void modem_operator_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

    /* Small default PHY header (type 1) in case caller didn't provide one. */
    static uint8_t default_phyheader[5] = {0};
    uint8_t *phyheader;
    uint32_t phyheader_len;

    while (1) {
        /* Prefer TX: try to get a TX packet first (short timeout) */
        struct tx_packet *pkt = tx_fifo_get(K_MSEC(100));
        if (pkt) {
            /* Wait until modem is available */
            while (!modem_operator_is_modem_free()) {
                k_msleep(1);
            }

            /* Mark modem busy: actual modem handler will also call modem_op_start and keep state consistent */
            modem_operator_set_modem_free(false);

            // LOG_INF("modem_operator: sending packet len=%d tx_srdid=0x%X", (int)pkt->len, pkt->transmitter_srdid);

            /* Use packet's stored header, or default if none provided */
            if (pkt->header_len > 0) {
                phyheader = pkt->header_data;
                phyheader_len = pkt->header_len;
            } else {
                phyheader = default_phyheader;
                phyheader_len = 5;
            }

            // LOG_INF("modem_operator: sending with len=%d", pkt->header_len);
            // LOG_INF("modem_operator: sending with PHY header len=%d", phyheader_len);
            // LOG_INF("Data to send:");
            // for (size_t i = 0; i < pkt->len; i++) {
            //     LOG_INF(" %02X", pkt->data[i]);
            // }
            // LOG_INF("\n");
            // LOG_INF("PHY Header to send:");
            // for (size_t i = 0; i < phyheader_len; i++) {
            //     LOG_INF(" %02X", phyheader[i]);
            // }
            // LOG_INF("\n");

            modem_tx(pkt->data, pkt->len, pkt->header_data, phyheader_len, pkt->scheduled);

            /* Return packet to pool */
            tx_fifo_free(pkt);

            /* Do NOT mark modem free here: modem_handler will call modem_op_complete and notify us. */
            continue;
        }

        /* No TX pending: process pending RX requests (if any) */
        struct rx_request req;
        if (k_msgq_get(&rx_req_q, &req, K_NO_WAIT) == 0) {
            /* If modem is free, start RX immediately, otherwise requeue (try later) */
            if (modem_operator_is_modem_free()) {
                modem_operator_set_modem_free(false);
                // LOG_INF("modem_operator: starting RX mode=%d time_ms=%d", req.mode, req.time_ms);
                modem_rx(req.mode, req.time_ms);
                /* modem_handler will mark modem free on completion */
            } else {
                /* Requeue: put back to the queue tail (drop if full) */
                if (k_msgq_put(&rx_req_q, &req, K_NO_WAIT) != 0) {
                    LOG_WRN("modem_operator: rx queue full, dropping request");
                }
            }
            continue;
        }

        /* Nothing to do, yield */
        k_msleep(10);
    }
}

K_THREAD_DEFINE(modem_operator_tid, 2048, modem_operator_thread, NULL, NULL, NULL,
                6, 0, 0);
