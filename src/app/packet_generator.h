#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H

#include <stddef.h>
#include <stdint.h>


void sendBeacon(void);
void sendAssocReq(void);
void sendAssocResp(void);
void sendData(void);
void sendBroadcastIndication(void);
void sendKeepAliveIE(void);
void sendDummyData(void);

#endif