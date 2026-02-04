#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/timeutil.h>
#include <modem/nrf_modem_lib.h>
#include <nrf_modem_dect_phy.h>
#include "utils.h"
#include "../config/device_config.h"


uint16_t generate_random_id(uint16_t seed) {

    // Get uptime since boot (ms)
    uint32_t uptime = k_uptime_get_32();
    uint16_t id;

    if(seed != 0){
        uptime = seed;
    }

    if((uptime % 10) > 6){
        uint16_t mixed = (uptime ^ (uptime >> 8) ^ (uptime << 3)) & 0xFFFF;

        // Reduce range to 0x0001..0xFFFE
        id = (mixed % (0xFFFE - 0x0001 + 1)) + 0x0001;
    }
    else if((uptime % 10) > 3 && (uptime % 10) <= 6)
    {
        // Mix seed with uptime
        uint16_t mixed = (uptime ^ uptime ^ (uptime >> 3) ^ (uptime << 5)) & 0xFFFF;

        // Reduce range to 0x0001..0xFFFE
        id = (mixed % (0xFFFE - 0x0001 + 1)) + 0x0001;
    }
    else
    {
        uptime = (uptime >> 1) ^ (-(uptime & 1u) & 0xB400u);

        // Reduce range to 0x0001..0xFFFE
        id = (uptime % (0xFFFE - 0x0001 + 1)) + 0x0001;
    }

    return id;
}


uint32_t generate_long_RDID(uint16_t SRDID, uint32_t networkID){

    uint32_t LRDID;
    LRDID = (SRDID << 16) | ((SRDID ^ networkID) & 0xFFFF);

    // Ensure LRDID is within range 0x00000001..0xFFFFFFFD
    while (LRDID < 0x00000001 || LRDID > 0xFFFFFFFD) {
        LRDID = LRDID - 0x00000005;
    }

    return LRDID;
}

int generateRandomNumber(int maxValue) {

    // Get uptime since boot (ms)
    uint32_t uptime = k_uptime_get_32();
    int ran;

    srand(uptime);
    ran = rand();
    
    // Scale to range 1..maxValue
    ran = (ran % maxValue) + 1;

    return ran;
}



// Example generator that works
// int generadorNumerosAleatorios(int semilla) {
    
//     srand(1234); // Initialize PRNG with seed 1234

//     for (int i = 0; i < 5; i++) {
//         printf("%d\n", rand()); // Generate and print a random number
//     }
// }