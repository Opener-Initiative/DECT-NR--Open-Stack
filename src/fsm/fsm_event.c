#include "fsm_event.h"

// Increased depth to reduce drops under bursts. Be mindful of RAM usage
// since struct fsm_event contains a 512-byte payload.
K_MSGQ_DEFINE(fsm_event_q, sizeof(struct fsm_event), 20, 4);

int fsm_event_post(struct fsm_event *evt) {
    return k_msgq_put(&fsm_event_q, evt, K_NO_WAIT);
}

int fsm_event_get(struct fsm_event *evt, k_timeout_t timeout) {
    return k_msgq_get(&fsm_event_q, evt, timeout);
}
