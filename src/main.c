// /******************************************************************************

// DECT NR+ implementation in progress
// Property of Universidad Politécnica de Cartagena, QARTIA Department.
// Author: Iván Tomás García (ivan.tomas@edu.upct.es)
// Start project date: 4/3/2023

// ****************************************************************************
// TODO:
// - A lot
// ****************************************************************************/


#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app/app_state.h"
#include "drivers/uart_handler.h"
#include "drivers/gpio_handler.h"
#include "drivers/modem_handler.h"
#include "drivers/sensor_bme280.h"
#include "drivers/sensor_htu21d.h"
// #include "drivers/display_handler.h"
#include "common/utils.h"
#include "common/data_buffers.h"
#include "config/device_config.h"
#include "app/applications.h"

LOG_MODULE_REGISTER(app);

static void app_run_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    app();
}

K_THREAD_DEFINE(app_run_tid, 4096, app_run_thread, NULL, NULL, NULL,
                5, 0, 0);



int main(void)
{

    while (1)
    {
        //// YOUR APPLICATION FUNCTION HERE ////









        /// Example functions from applications.c ///
        if(IS_GATEWAY)
        {
            // print_sensor_data();
            print_data();
        }
        else
        {
            if(read_sensors_task() != 0)
                generate_dummy_data();
        }
        //// Leave a thread sleep to allow other threads to run
        k_msleep(1);
    }

    return 0;
}

