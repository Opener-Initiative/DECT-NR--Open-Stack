#include "network.h"
#include <zephyr/logging/log.h>
#include <string.h>
#include "../config/device_config.h"
#include "../protocol/procedures.h"
#include "../common/utils.h"
#include "../common/sfn_allocator.h"

LOG_MODULE_REGISTER(network);

// Variables globales
struct Network registeredNetworks[10];
int network_index = 1;
int registeredNetworks_index = 0;
bool associated = false;
static uint16_t rd_id;
static uint32_t networkID;
struct Network networks[10];
static bool isFT = false;
static struct TXParams tp;
static bool ftOperationsDone = false;



// Network inicialization
void network_init(void)
{
    /// Initialize SFN pool
    if (sfn_pool_init() != 0)   LOG_INF("Failed to initialize SFN pool");

    //// Get device ID (SRDID)
    if(RD_ID == NULL) {
        rd_id = generate_random_id(0);
    }
    else {
        rd_id = (uint16_t) RD_ID;
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
    networks[0].network_FT = generate_long_RDID(rd_id, networkID);
    networks[0].networkID = networkID;                                 // Assign the network ID to the network
    networks[0].own_LRDID = networks[0].network_FT; // Assign the long RD ID to the network

    if(networks[0].own_LRDID == 0){
        // LOG_INF("No LRDID associated, creating one");
        networks[0].own_LRDID = generate_long_RDID(rd_id, networkID);
    }
    LOG_INF("LRDID: %x", networks[0].own_LRDID);

    tp.isGW = IS_GATEWAY;
    tp.S_SRDID = rd_id;
    tp.S_LRDID = networks[0].own_LRDID;
    tp.SnetworkID = networkID;
    if(IS_GATEWAY)
    {
        // LOG_INF("Configured as Gateway");
        tp.Ring_Level = 0; // For FT
        tp.isFT = true;
    }
    else
    {
        // LOG_INF("Configured as default");
        tp.isFT = IS_GATEWAY;
        tp.Ring_Level = RING_LEVEL; 
    }

    if(FIXED_SFN >= 0)
    {
        networks[0].SFN = FIXED_SFN;        //// To modify once more networks are supported
    }
}

// Actualiza la lista de dispositivos conectados a la red
void deviceListUpdate(void)
{
    for (int i = 0; i < networks[0].n_devices; i++) {
        networks[0].list_LRDID[i][1]--;

        if (networks[0].list_LRDID[i][1] <= 0) {
            uint32_t removed_lrdid = networks[0].list_LRDID[i][0];
            LOG_INF("Removing device with LRDID: %x", removed_lrdid);

            for (int j = i; j < networks[0].n_devices - 1; j++) {
                networks[0].list_LRDID[j][0] = networks[0].list_LRDID[j + 1][0];
                networks[0].list_LRDID[j][1] = networks[0].list_LRDID[j + 1][1];
            }

            networks[0].list_LRDID[networks[0].n_devices - 1][0] = 0;
            networks[0].list_LRDID[networks[0].n_devices - 1][1] = 0;

            networks[0].n_devices--;

            for (int k = 0; k < registeredNetworks_index; k++) {
                if ((registeredNetworks[k].network_FT >> 16) ==
                    (removed_lrdid >> 16)) {

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
            }
            i--;
        }
    }
    if (networks[0].n_devices == 0) {
        associated = false;
        LOG_INF("No devices connected, not associated-----------------------------------------------------");
    }
}

// Verifica si un LRDID recibido pertenece a mis redes registradas
bool checkDestination(uint32_t LRDID)
{
    for (int i = 0; i < network_index; i++) {
        if (networks[i].own_LRDID == LRDID) {
            LOG_INF("Message for this device");
            LOG_INF("My LRDID: %X", networks[i].own_LRDID);
            LOG_INF("Dest LRDID: %X", LRDID);
            return true;

        }
    }
    return false;
}

// Registra una red si no estaba registrada previamente
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
    networks[network_index].own_LRDID = generate_long_RDID(rd_id, networkID);
    // YA NO (own_LRDID se calcula fuera con generate_long_RDID())
    network_index++;
    return false;
}

bool deviceCheck(uint16_t SRDID)
{
    for (int i = 0; i < networks[0].n_devices; i++) {
        if ((networks[0].list_LRDID[i][0] >> 16 ) == SRDID) {
        LOG_INF("Device already associated");

        struct TXParams *tp = get_TP();
        tp->D_LRDID = networks[0].list_LRDID[i][0]; // Utiliza el RDID del paquete para enviar la respuesta
        tp->SFN = networks[0].list_LRDID[i][2]; // SFN of the device that is already associated

        networks[0].list_LRDID[i][1] = 3; // Reset the timer of the device that is already associated
        LOG_INF("Updated timer for device with LRDID: %X", networks[0].list_LRDID[i][0]);
        return true;
        }
    }
    return false;
}

// Empaqueta la información de red en un mensaje
int getNetworkInfo(uint8_t *message)
{
    struct StatusDevNet sdn;
    sdn.IsFT = false; // se puede actualizar fuera
    sdn.NetworkID = networks[0].networkID;
    sdn.n_devices = networks[0].n_devices;
    sdn.DataLength = networks[0].n_devices;
    sdn.LRDID[0] = networks[0].own_LRDID;
    for (int i = 1; i < networks[0].n_devices + 1; i++) {
        sdn.LRDID[i] = networks[0].list_LRDID[i - 1][0];
    }
    
    //// Print network info for debugging
    printk("getNetworkInfo\n");
    printk("Network ID: %X\n", sdn.NetworkID);
    printk("N devices: %d\n", sdn.n_devices);
    for (int i = 0; i < sdn.n_devices + 1; i++) 
    {
        printk("LRDID[%d]: %X\n", i, sdn.LRDID[i]);
    }



    /* Clear packed buffer then build it from the struct to avoid leftover data */
    memset(sdn.sdn, 0, sizeof(sdn.sdn));
    getStatusDevNet(&sdn);
    memcpy(message, &sdn.sdn, 50);
    return 1;
}

int setNetworkInfo(uint8_t *message)
{
    // `message` is expected to point at the packed StatusDevNet buffer (sdn.sdn).
    // Callers pass &_rxData[4], so the first byte of the status block is message[0].
    if (message[0] != 0) {
        struct StatusDevNet statusDevNet;
        memset(&statusDevNet, 0, sizeof(statusDevNet));
        memcpy(&statusDevNet.sdn, message, 50);
        setStatusDevNet(&statusDevNet);

        // for (int i = 0; i < 50; i++) {
        //   printk("%02x ", statusDevNet.sdn[i]);
        // }
        // printk("\n");

        // Store data in the local variable
        // printk("Data length %d\n", statusDevNet.DataLength);
        // printk("N Devices %d\n", statusDevNet.n_devices);

        // Check if the networkID already exists in registeredNetworks
        for (int i = 0; i < registeredNetworks_index; i++) {
            if (registeredNetworks[i].networkID == statusDevNet.NetworkID) {
                LOG_INF("Network ID %X already exists in registeredNetworks", statusDevNet.NetworkID);
                return 0;
            }
        }
        registeredNetworks[registeredNetworks_index].n_devices = statusDevNet.n_devices;
        registeredNetworks[registeredNetworks_index].networkID = statusDevNet.NetworkID;
        registeredNetworks[registeredNetworks_index].isGW = statusDevNet.IsGW;
        registeredNetworks[registeredNetworks_index].network_FT = statusDevNet.LRDID[0];

        for (int i = 0; i < statusDevNet.n_devices; i++) {
            registeredNetworks[registeredNetworks_index].list_LRDID[i][0] = statusDevNet.LRDID[i+1];
        }

        registeredNetworks_index++;

        LOG_INF("N registered networks: %d", registeredNetworks_index);

        // // Print the registered network details
        for (int i = 0; i < registeredNetworks_index; i++) {
          LOG_INF("Registered Network %d:", i + 1);
          LOG_INF("Network ID: %X", registeredNetworks[i].networkID);
          LOG_INF("FT ID: %X", registeredNetworks[i].network_FT);
          LOG_INF("Number of Devices: %d", registeredNetworks[i].n_devices);

          // Print up to 3 devices from the list_LRDID
          int devices_to_print = registeredNetworks[i].n_devices < 3 ? registeredNetworks[i].n_devices : 3;
          for (int j = 0; j < devices_to_print; j++) {
            LOG_INF("Device %d LRDID: %X", j + 1, registeredNetworks[i].list_LRDID[j][0]);
          }
        }
            

        return 1;
    }
    else {
        LOG_INF("Data at message[0] is zero (no network info)");
        LOG_INF("Registering that %x is alive", 1 );

        // Store data in the local variable
        return 0;

    }
}

void printNetworkInfo()
{
    // printk("--------OWN NETWORK--------\n");
    // printk("Network ID: %X\n", networks[0].networkID);
    // printk("FT ID: %X\n", networks[0].network_FT);
    // printk("N devices %d\n", networks[0].n_devices);
    printk("%X\n", networks[0].networkID);
    printk("%X\n", networks[0].network_FT);
    for (size_t i = 0; i < networks[0].n_devices; i++)
    {
        printk("%X\n", networks[0].list_LRDID[i][0]);
        // printk("LRDID n %d: %X\n", i, networks[0].list_LRDID[i][0]);
    }
    printk("END_N:0\n");

    if(IS_GATEWAY && registeredNetworks_index != 0)
    {
        // printk("--------OTHERS NETWORKS--------\n");
        for (size_t i = 0; i < registeredNetworks_index; i++)
        {
        // printk("Network ID: %X\n", registeredNetworks[i].networkID);
        // printk("FT ID: %X\n", registeredNetworks[i].network_FT);
        printk("%X\n", registeredNetworks[i].networkID);
        printk("%X\n", registeredNetworks[i].network_FT);
        for (size_t j = 0; j < registeredNetworks[i].n_devices; j++)
        {
            printk("%X\n", registeredNetworks[i].list_LRDID[j][0]);
            // printk("LRDID n %d: %X\n", j, registeredNetworks[i].list_LRDID[j][0]);
        }
        printk("END_N:1\n");
        }
    }
    // printk("-------------------------------\n\n");
    // k_msleep(3000);

    
    printk("SAVE\n");
}

void printFancyNetworkInfo()
{
    printk("--------OWN NETWORK--------\n");
    printk("Network ID: %X\n", networks[0].networkID);
    printk("FT ID: %X\n", networks[0].network_FT);
    printk("N devices %d\n", networks[0].n_devices);
    for (size_t i = 0; i < networks[0].n_devices; i++)
    {
        printk("LRDID n %d: %X\n", i, networks[0].list_LRDID[i][0]);
    }

    LOG_INF("Others networks registered: %d", registeredNetworks_index);
    if(IS_GATEWAY && registeredNetworks_index != 0)
    {
        printk("--------OTHERS NETWORKS--------\n");
        for (size_t i = 0; i < registeredNetworks_index; i++)
        {
            printk("Network ID: %X\n", registeredNetworks[i].networkID);
            printk("FT ID: %X\n", registeredNetworks[i].network_FT);
            for (size_t j = 0; j < registeredNetworks[i].n_devices; j++)
            {
                printk("LRDID n %d: %X\n", j, registeredNetworks[i].list_LRDID[j][0]);
            }
        }
    }
    printk("-------------------------------\n\n");

    
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

int get_SFN(int network_index)
{
    // printk("Getting SFN %d for network index %d\n", networks[network_index].SFN, network_index);
    return networks[network_index].SFN;             // To modify once more networks are supported
}





