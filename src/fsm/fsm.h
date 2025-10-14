#ifndef FSM_H
#define FSM_H

#include <stddef.h>
#include <stdint.h>

void rx_consumer_init(void);
void process_rx_packet(const uint8_t *data, size_t len);

void sendBeacon(void);
void sendAssocReq(void);
void sendAssocResp(void);
void sendData(void);
int  ftOperations(void);

#endif