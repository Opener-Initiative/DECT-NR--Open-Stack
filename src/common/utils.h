#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <modem/nrf_modem_lib.h>
#include <nrf_modem_dect_phy.h>



uint16_t generate_random_id(uint16_t seed);
uint32_t generate_long_RDID(uint16_t SRDID, uint32_t networkID);
int generateRandomNumber(int maxValue);