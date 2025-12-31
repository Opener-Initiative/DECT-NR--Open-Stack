#ifndef MODEM_OPERATOR_H
#define MODEM_OPERATOR_H

#include <stdbool.h>

#include <stdint.h>

/* Initialize modem operator (called at app startup). Thread is defined internally. */
void modem_operator_init(void);

/* Set modem availability flag (called from modem code) */
void modem_operator_set_modem_free(bool free);

/* Query modem availability */
bool modem_operator_is_modem_free(void);

/* Request the modem to start RX. Returns 0 on success, -ENOMEM if queue full. */
int modem_operator_request_rx(uint32_t rxMode, int time_ms);

#endif // MODEM_OPERATOR_H
