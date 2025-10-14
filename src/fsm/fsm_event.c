#include "fsm_event.h"

K_MSGQ_DEFINE(fsm_event_q, sizeof(struct fsm_event), 10, 4);

int fsm_event_post(struct fsm_event *evt) {
    k_msgq_put(&fsm_event_q, evt, K_NO_WAIT);
    return 0;
}

int fsm_event_get(struct fsm_event *evt, k_timeout_t timeout) {
    return k_msgq_get(&fsm_event_q, evt, timeout);
}
