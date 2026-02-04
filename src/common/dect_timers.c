#include "dect_timers.h"
#include <string.h>
#include <zephyr/logging/log.h>
#include "../app/app_state.h"
#include "../fsm/fsm_event.h"

LOG_MODULE_REGISTER(dect_timers);

#define MAX_TIMER_EVENTS 10

//// Message queue for timer events
K_MSGQ_DEFINE(timer_msgq, sizeof(struct timer_event), MAX_TIMER_EVENTS, 4);
//// Message queue for deferred timer events
K_MSGQ_DEFINE(timer_pending_msgq, sizeof(struct timer_event), MAX_TIMER_EVENTS, 4);

static struct k_timer beacon_timer, assoc_msg_timer, broadcast_timer, rx_timer, sfn_timer, ft_timer, isolation_timer, 
ft_data_timer, ft_assoc_timer, assoc_win_timer, data_win_timer;

//// Store configured priority per timer type
// Allocate enough entries to include FT_ASSOC_TIMER (and avoid overflow
// when saving priorities for types up to FT_ASSOC_TIMER).
static uint8_t timer_priority_map[DATA_WIN + 1];

//// Flag set by application while processing a packet
static atomic_t processing_flag = ATOMIC_INIT(0);

/// @brief Generic handler called on timer expiry
/// @param timer_id 
static void timer_expiry_handler(struct k_timer *timer_id)
{
    struct timer_event evt = {0};
    evt.timestamp = k_uptime_get();

    if (timer_id == &beacon_timer) {
        evt.type = BEACON_TIMER;
        // evt.priority = 2;
    } else if (timer_id == &assoc_msg_timer) {
        evt.type = ASSOC_MSG_TIMER;
    } else if (timer_id == &broadcast_timer) {
        evt.type = BROADCAST_TIMER;
    } else if (timer_id == &rx_timer) {
        evt.type = RX_TIMER;
    } else if (timer_id == &ft_assoc_timer) {
        evt.type = FT_ASSOC_TIMER;
    } else if (timer_id == &sfn_timer) {
        evt.type = SFN_TIMER;
    } else if (timer_id == &ft_timer) {
        evt.type = SFN_TIMER;
    } else if (timer_id == &isolation_timer) {
        evt.type = ISOLATION_TIMER;
    } else if (timer_id == &ft_data_timer) {
        evt.type = FT_DATA_TIMER;
    } else if (timer_id == &assoc_win_timer) {
            evt.type = ASSOC_WIN;
    } else if (timer_id == &data_win_timer) {
            evt.type = DATA_WIN;
    } else {
        //// Unknown timer, ignore
        return;
    }

    //// Fill priority from map
    if (evt.type <= FT_DATA_TIMER) {
        evt.priority = timer_priority_map[evt.type];
    } else {
        evt.priority = 0;
    }

    //// If currently processing a packet and this event is not priority 0, defer it
    if (atomic_get(&processing_flag) && evt.priority != 0) {
        k_msgq_put(&timer_pending_msgq, &evt, K_NO_WAIT);
        return;
    }

    k_msgq_put(&timer_msgq, &evt, K_NO_WAIT);
}

void timers_init(void)
{
    k_timer_init(&beacon_timer, timer_expiry_handler, NULL);
    k_timer_init(&assoc_msg_timer, timer_expiry_handler, NULL);
    k_timer_init(&broadcast_timer, timer_expiry_handler, NULL);
    k_timer_init(&rx_timer, timer_expiry_handler, NULL);
    k_timer_init(&sfn_timer, timer_expiry_handler, NULL);
    k_timer_init(&ft_timer, timer_expiry_handler, NULL);
    k_timer_init(&isolation_timer, timer_expiry_handler, NULL);
    k_timer_init(&ft_data_timer, timer_expiry_handler, NULL);
    k_timer_init(&ft_assoc_timer, timer_expiry_handler, NULL);
    k_timer_init(&assoc_win_timer, timer_expiry_handler, NULL);
    k_timer_init(&data_win_timer, timer_expiry_handler, NULL);
}

int timers_start(enum timer_event_type type, k_timeout_t timeout, uint8_t priority)
{
    struct k_timer *t = NULL;
    switch (type) {
    case BEACON_TIMER: t = &beacon_timer; break;
    case ASSOC_MSG_TIMER: t = &assoc_msg_timer; break;
    case BROADCAST_TIMER: t = &broadcast_timer; break;
    case RX_TIMER: t = &rx_timer; break;
    case SFN_TIMER: t = &sfn_timer; break;
    case ISOLATION_TIMER: t = &isolation_timer; break;
    case FT_DATA_TIMER: t = &ft_data_timer; break;
    case FT_ASSOC_TIMER: t = &ft_assoc_timer; break;
    case ASSOC_WIN: t = &assoc_win_timer; break;
    case DATA_WIN: t = &data_win_timer; break;
    }

    if (!t) return -EINVAL;
    //// Save priority if it is in range
    if (type <= FT_ASSOC_TIMER) timer_priority_map[type] = priority;

    k_timer_start(t, timeout, K_NO_WAIT);
    /* Avoid passing k_timeout_t (an aggregate) to the log macros which
     * can't package aggregate types. Log the timer type and priority
     * instead of the raw k_timeout_t value. If you want the numeric
     * milliseconds, convert the timeout to ms explicitly before logging.
     */
    LOG_INF("Started timer: %d (priority=%d)", type, priority);
    return 0;
}

int timers_stop(enum timer_event_type type)
{
    struct k_timer *t = NULL;
    switch (type) {
    case BEACON_TIMER: t = &beacon_timer; break;
    case ASSOC_MSG_TIMER: t = &assoc_msg_timer; break;
    case BROADCAST_TIMER: t = &broadcast_timer; break;
    case RX_TIMER: t = &rx_timer; break;
    case SFN_TIMER: t = &sfn_timer; break;
    case ISOLATION_TIMER: t = &isolation_timer; break;
    case FT_DATA_TIMER: t = &ft_data_timer; break;
    case FT_ASSOC_TIMER: t = &ft_assoc_timer; break;
    case ASSOC_WIN: t = &assoc_win_timer; break;
    case DATA_WIN: t = &data_win_timer; break;
    }

    if (!t) return -EINVAL;

    k_timer_stop(t);
    return 0;
}

//// Consumer: get next event
struct timer_event timers_get_event(k_timeout_t timeout)
{
    struct timer_event evt = {0};
    k_msgq_get(&timer_msgq, &evt, timeout);
    return evt;
}

void timers_set_processing(bool processing)
{
    if (processing) {
        atomic_set(&processing_flag, 1);
    } else {
        atomic_set(&processing_flag, 0);
    }
}

void timers_process_pending(void)
{
    struct timer_event evt;
    
    //// Move all pending events to main queue in FIFO order
    while (k_msgq_get(&timer_pending_msgq, &evt, K_NO_WAIT) == 0) {
        k_msgq_put(&timer_msgq, &evt, K_NO_WAIT);
    }
}

void timer_consumer_thread(void)
{
    while (1) {
        struct timer_event evt = timers_get_event(K_FOREVER);
        // app_on_timer_event(&evt);   // action delegated to app_state.c
        struct fsm_event f_evt = {
            .type = FSM_EVENT_TIMER,
            .timer = evt,
        };

        if (fsm_event_post(&f_evt) != 0) {
            LOG_WRN("FSM event queue full, dropping timer event");
}

    }
}

K_THREAD_DEFINE(timer_tid, 2048, timer_consumer_thread, NULL, NULL, NULL,
                5, 0, 0);