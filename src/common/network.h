#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "../protocol/procedures.h"

// // Main network structure
// struct Network {
//     uint32_t networkID;
//     uint32_t network_FT;
//     uint32_t own_LRDID;
//     int n_devices;
//     bool isGW;
//     int list_LRDID[50][3]; // [LRDID, counter, SFN]
//     int SFN;
// };

// Network global variables
extern struct Network networks[10];
extern struct Network registeredNetworks[10];
extern int network_index;
extern int registeredNetworks_index;
extern bool associated;

// Network management functions
void network_init(void);
void deviceListUpdate(void);
bool checkDestination(uint32_t LRDID);
bool networkCheck(uint32_t networkID, uint32_t FT);
bool deviceCheck(uint16_t SRDID);
int getNetworkInfo(uint8_t *message);
int setNetworkInfo(uint8_t *message);
void printNetworkInfo(void);
void printFancyNetworkInfo(void);
bool is_network_empty(void);
void deleteLastNetwork(void);
void set_associated(bool state);
bool is_associated(void);
void set_isFT(bool state);
bool is_isFT(void);
void update_TP(struct TXParams *new_tp);
struct TXParams* get_TP(void);

int get_SFN(int network_index);
uint16_t get_rd_id(void);

#endif // NETWORK_H
