#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

/* ========== DEVICE CONFIGURATION ========== */

/* Device role */
#define USING_GATEWAYS      true
#define IS_GATEWAY          false
// #define IS_COORDINATOR      false

/* Device identifiers */
#define RD_ID               NULL
#define NETWORK_ID          NULL

#define CARRIER             1664
#define MAX_NETWORKS        3
#define RING_LEVEL          0
#define DATA_LEN            700     // to modify for a complex MCS function
#define BEACON_PERIOD       2000    // in ms

#define FIXED_SFN               0      // -1 to disable, else fixed SFN value
#define BROADCAST_IND_PERIOD    3000    // in ms
#define SWITCH_DELAY            90     // in ms


//// DEBUG CONFIG
#define ENABLE_ASSOCIATION  true

#endif