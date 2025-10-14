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

    // Obtener el tiempo desde el arranque
    uint32_t uptime = k_uptime_get_32();
    uint16_t id;

    if(seed != 0){
        uptime = seed;
    }

    if((uptime % 10) > 6){
        uint16_t mixed = (uptime ^ (uptime >> 8) ^ (uptime << 3)) & 0xFFFF;

        // Reducir el rango entre 0x0001 y 0xFFFE
        id = (mixed % (0xFFFE - 0x0001 + 1)) + 0x0001;
    }
    else if((uptime % 10) > 3 && (uptime % 10) <= 6)
    {
        // Mezclar seed con uptime
        uint16_t mixed = (uptime ^ uptime ^ (uptime >> 3) ^ (uptime << 5)) & 0xFFFF;

        // Reducir el rango entre 0x0001 y 0xFFFE
        id = (mixed % (0xFFFE - 0x0001 + 1)) + 0x0001;
    }
    else
    {
        uptime = (uptime >> 1) ^ (-(uptime & 1u) & 0xB400u);

        // Reducir el rango entre 0x0001 y 0xFFFE
        id = (uptime % (0xFFFE - 0x0001 + 1)) + 0x0001;
    }

    return id;
}


uint32_t generate_long_RDID(uint16_t SRDID, uint32_t networkID){

    uint32_t LRDID;
    LRDID = (SRDID << 16) | ((SRDID ^ networkID) & 0xFFFF);

    // Verificar si LRDID está en el rango de 0x00000001 y 0xFFFFFFFD
    while (LRDID < 0x00000001 || LRDID > 0xFFFFFFFD) {
        LRDID = LRDID - 0x00000005;
    }

    return LRDID;
}

int generateRandomNumber(int maxValue) {

    // Obtener el tiempo desde el arranque
    uint32_t uptime = k_uptime_get_32();
    int ran;

    srand(uptime);
    ran = rand();
    
    // Reducir el rango entre 1 y maxValue
    ran = (ran % maxValue) + 1;

    return ran;
}



// Este generador funciona
// int generadorNumerosAleatorios(int semilla) {
    
//     srand(1234); // Inicializa el generador de números aleatorios con la semilla 1234

//     for (int i = 0; i < 5; i++) {
//         printf("%d\n", rand()); // Genera y muestra un número aleatorio
//     }
// }