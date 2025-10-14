#ifndef APP_STATE_H
#define APP_STATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct timer_event;

/// @brief FSM states - Estados de la máquina de estados
enum app_states {
    SEND_BEACON = 1,
    SEND_ASSOC_REQ = 3,
    SEND_ASSOC_RESP = 5,
    SEND_DATA = 7,
    SEND_BROADCAST_IND = 11,
    SEND_KAIE = 13,
    SEND_METRIC_REQ = 15,
    SEND_METRIC_RESP = 17,
    SEND_DEASSOC = 19,
    FT_OPERATIONS = 21,

    WAIT_BEACON = 2,
    WAIT_ASSOC_REQ = 4,
    WAIT_ASSOC_RESP = 6,
    WAIT_DATA = 8,
    WAIT_RX = 10,
    WAIT_KAIE = 12,
    WAIT_METRIC_REQ = 14,
    WAIT_METRIC_RESP = 16,
};

/// @brief Initialization of the application
void app_init(void);

/// @brief Main application loop. Blocking.
void app_run(void);

/// @brief Process incoming UART data
/// @param data 
/// @param len 
void app_on_uart_data(const uint8_t *data, size_t len);

/// @brief Process timer events
/// @param evt 
void app_on_timer_event(const struct timer_event *evt);

//// Controlled access to state variables
int app_get_state(void);
int app_get_next_state(void);
void app_set_next_state(int next);

//// For testing & debugging purposes
void app_test(void);

#endif
