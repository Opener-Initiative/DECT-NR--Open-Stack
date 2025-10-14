#include "network.h"
#include <zephyr/logging/log.h>
#include <string.h>
#include "../config/device_config.h"
#include "../protocol/procedures.h"
#include "../common/utils.h"
#include "../common/sfn_allocator.h"

LOG_MODULE_REGISTER(network);

// Global variables
struct Network registeredNetworks[10];
int network_index = 1;
int registeredNetworks_index = 0;
bool associated = false;
static uint16_t rd_id;
static uint32_t networkID;
struct Network networks[10];
static bool isFT = false;
static struct TXParams tp;



// Network inicialization
void network_init(void)
{
    //// Get device ID (SRDID)
    if(RD_ID == NULL) {
        rd_id = generate_random_id(0);
    }
    else {
        rd_id = (uint16_t)(uintptr_t)RD_ID;
    }
    LOG_INF("RD ID: %x", rd_id);

    //// Get Network ID
    networkID = NETWORK_ID;
    if(NETWORK_ID == NULL) {
        //// Get random Network ID (may be disposed if no connections are created)
        uint32_t rand_networkID = generate_random_id(0);
        rand_networkID = rand_networkID << 16 | generate_random_id(rand_networkID);
        networkID = rand_networkID;
    }
    LOG_INF("Network ID: %x\n", networkID);

    networks[0].n_devices = 0;
    networks[0].network_FT = rd_id;
    networks[0].networkID = networkID;                                 // Assign the network ID to the network
    networks[0].own_LRDID = generate_long_RDID(tp.S_SRDID, networkID); // Assign the long RD ID to the network

    if(networks[0].own_LRDID == 0){
        // LOG_INF("No LRDID associated, creating one");
        networks[0].own_LRDID = generate_long_RDID(tp.S_SRDID, networkID);
    }
    LOG_INF("LRDID: %x", networks[0].own_LRDID);

    tp.isGW = IS_GATEWAY;
    tp.S_SRDID = rd_id;
    tp.S_LRDID = networks[0].own_LRDID;
    tp.SnetworkID = networkID;
    if(IS_GATEWAY)
    {
        LOG_INF("Configured as Gateway");
        tp.Ring_Level = 0; // For FT
        tp.isFT = true;
    }
    else
    {
        LOG_INF("Configured as default");
        tp.isFT = IS_GATEWAY;
        tp.Ring_Level = RING_LEVEL; 
    }
}

// Actualiza la lista de dispositivos conectados a la red
void deviceListUpdate(void)
{
    for (int i = 0; i < networks[0].n_devices; i++) {
        networks[0].list_LRDID[i][1]--;

        if (networks[0].list_LRDID[i][1] <= 0) {
            LOG_INF("Removing device with LRDID: %X", networks[0].list_LRDID[i][0]);

            for (int j = i; j < networks[0].n_devices - 1; j++) {
                networks[0].list_LRDID[j][0] = networks[0].list_LRDID[j + 1][0];
                networks[0].list_LRDID[j][1] = networks[0].list_LRDID[j + 1][1];
            }

            networks[0].list_LRDID[networks[0].n_devices - 1][0] = 0;
            networks[0].list_LRDID[networks[0].n_devices - 1][1] = 0;

            networks[0].n_devices--;

            for (int k = 0; k < registeredNetworks_index; k++) {
                if ((registeredNetworks[k].network_FT >> 16) ==
                    (networks[0].list_LRDID[i][0] >> 16)) {

                    LOG_INF("Removing registered network with FT ID: %X",
                            registeredNetworks[k].network_FT);

                    for (int l = k; l < registeredNetworks_index - 1; l++) {
                        registeredNetworks[l] = registeredNetworks[l + 1];
                    }
                    memset(&registeredNetworks[registeredNetworks_index - 1], 0,
                           sizeof(struct Network));
                    registeredNetworks_index--;
                    k--;
                }
                if (networks[0].n_devices == 0) {
                    associated = false;
                }
            }
            i--;
        }
    }
}

//// Verifies if a received LRDID belongs to my registered networks
bool checkDestination(uint32_t LRDID)
{
    for (int i = 0; i < network_index; i++) {
        if (networks[i].own_LRDID == LRDID) {
            return true;
        }
    }
    return false;
}

//// Registers a network if it was not previously registered
bool networkCheck(uint32_t networkID, uint32_t FT)
{
    uint32_t nID;
    for (int i = 0; i < network_index; i++) {
        nID = ((networkID << 8) & 0xFFFFFF00) | (/* ShortNetworkID */ 0);
        if (networks[i].networkID == nID) {
            return true;
        }
    }
    networks[network_index].networkID = nID;
    networks[network_index].network_FT = FT;
    // own_LRDID is calculated outside with generate_long_RDID()
    network_index++;
    return false;
}

bool deviceCheck(uint32_t LRDID)
{
    for (int i = 0; i < networks[0].n_devices; i++) {
        if (networks[0].list_LRDID[i][0] == LRDID) {
        LOG_INF("Device already associated");

        struct TXParams *tp = get_TP();
        tp->D_LRDID = networks[0].list_LRDID[i][0]; // Utiliza el RDID del paquete para enviar la respuesta
        tp->SFN = networks[0].list_LRDID[i][2]; // SFN of the device that is already associated

        networks[0].list_LRDID[i][1] = 3; // Reset the timer of the device that is already associated
        
        return true;
        }
    }
    return false;
}

void deleteLastNetwork(void)
{
    if (network_index > 1) {
        memset(&networks[network_index - 1], 0, sizeof(struct Network));
        network_index--;
    }
}

bool is_network_empty(void)
{
    return networks[0].n_devices == 0;
}

bool is_associated(void)
{
    return associated;
}

void set_associated(bool state)
{
    associated = state;
}

bool is_isFT(void)
{
    return isFT;
}

void set_isFT(bool state)
{
    isFT = state;
}

void update_TP(struct TXParams *new_tp)
{
    tp = *new_tp;
}

struct TXParams* get_TP(void)
{
    return &tp;
}

uint16_t get_rd_id(void)
{
    return rd_id;
}





