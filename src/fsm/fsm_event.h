#pragma once
#include <zephyr/kernel.h>
#include "../common/dect_timers.h"

enum fsm_event_type {
    FSM_EVENT_PACKET_RX,
    FSM_EVENT_TIMER,
    FSM_EVENT_UART,
};

struct fsm_event {
    enum fsm_event_type type;
    union {
        struct {
            uint8_t data[512];
            size_t len;
        } rx;
        struct timer_event timer;
    };
};

int fsm_event_post(struct fsm_event *evt);
int fsm_event_get(struct fsm_event *evt, k_timeout_t timeout);
