
#pragma once
#include <zephyr/kernel.h>
#include <stdint.h>

enum timer_event_type {
    BEACON_TIMER,
    ASSOC_MSG_TIMER,
    BROADCAST_TIMER,
    RX_TIMER,
    ISOLATION_TIMER,
    FT_DATA_TIMER,
    FT_ASSOC_TIMER

};


struct timer_event {
    enum timer_event_type type;
    uint64_t timestamp;
    uint8_t priority;   // Not used for now
};

void timers_init(void);
int timers_start(enum timer_event_type type, k_timeout_t timeout, uint8_t priority);
int timers_stop(enum timer_event_type type);

/// @brief Event consummer for timers
struct timer_event timers_get_event(k_timeout_t timeout);

/// @brief Function called by timer system when a timer expires
void app_on_timer_event(const struct timer_event *evt);

/// @brief Set processing state for timer events
/// @param processing 
void timers_set_processing(bool processing);

/// @brief Process any pending (deferred) timer events
void timers_process_pending(void);
