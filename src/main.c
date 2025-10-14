/******************************************************************************

DECT NR+ implementation in progress
Autor: Iván Tomás García (ivan.tomas@edu.upct.es)
Start project date: 4/3/2023

****************************************************************************
TODO:
- A lot
****************************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app/app_state.h"
#include "drivers/gpio_handler.h"
#include "drivers/modem_handler.h"
#include "drivers/display_handler.h"
#include "common/utils.h"

LOG_MODULE_REGISTER(app);

int main(void)
{
    LOG_INF("DECT NR+ reorg: arrancando");

    gpio_init();
    // uart_init();
    modem_init();
    // sensor_bme280_init();
    sensor_htu21d_init();
    display_init();
    app_init();

    // app_test();
    
    app_run();

    return 0;
}
