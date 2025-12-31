#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../app/app_state.h"
#include "../drivers/uart_handler.h"
#include "../drivers/gpio_handler.h"
#include "../drivers/modem_handler.h"
#include "../drivers/sensor_bme280.h"
#include "../drivers/sensor_htu21d.h"
#include "../common/utils.h"
#include "../common/data_buffers.h"
#include "../config/device_config.h"
#include "../common/rx_fifo.h"


LOG_MODULE_REGISTER(applications);

void print_data(void)
{
    if(!IS_GATEWAY) return;
    static uint8_t data_buffer[APP_RX_BUFFER_SIZE];

    while (1) {
        if(!app_rx_buffer_available()) {
            // LOG_INF("No data available from RX buffer");
            k_msleep(1);
            continue;
        }
        // LOG_INF("Data available from RX buffer (dropped=%d pool_exhausted=%d)", rx_fifo_dropped_count(), rx_pool_exhausted_count());
        int err = app_rx_buffer_read(data_buffer, sizeof(data_buffer));
        if (err < 0) {
            LOG_INF("No data available from RX buffer: %d", err);
            k_msleep(100);
            continue;
        }

        if (err > 0) {
            // LOG_INF("Received from %d", data_buffer[0]);
            for (int i = 0; i < err; ++i) {
                printk("%02X", data_buffer[i]);
                if ((i + 1) % 16 == 0) {
                    printk("\n");
                } else {
                    printk(" ");
                }
            }
            printk("\n");
        }
    }
}

void generate_dummy_data()
{
    while(1)
    {
        if(!IS_GATEWAY)
        {
            if(!app_tx_buffer_available())
            {
                uint8_t dummy_data[693];
                for(size_t i = 0; i < sizeof(dummy_data); i++)
                {
                    dummy_data[i] = (uint8_t)(i & 0xFF);
                }
                app_tx_buffer_write(dummy_data, sizeof(dummy_data));
            }
        }
        k_msleep(100); // Send every 5 seconds
    }
}




float rx_temperature, rx_humidity;
int read_sensors_task(void)
{
    float temp, hum;

    if(sensor_htu21d_init() != 0)
    {
        LOG_ERR("Failed to initialize HTU21D sensor");
        return -1;
    }
    

    while (1) {
        hum = get_htu21d_humidity();
        temp = get_htu21d_temperature();
        LOG_INF("HTU21D - Temp: %.2f C, Hum: %.2f %%",temp,hum);
        // LOG_INF("app_tx_buffer_available: %d", app_tx_buffer_available());

        if(!app_tx_buffer_available())
        {
            uint8_t sensor_data[8];
            memcpy(&sensor_data[0], &temp, sizeof(float));
            memcpy(&sensor_data[4], &hum, sizeof(float));
            app_tx_buffer_write(sensor_data, sizeof(sensor_data));
            LOG_WRN("Sensor data written to TX buffer");
        }
        else
        {
            // LOG_WRN("TX buffer full, skipping sensor data write");
        }

        k_msleep(1000); // Read every 1 second
    }
}

float get_rx_temp(void)
{
    return rx_temperature;
}

float get_rx_hum(void)
{
    // LOG_INF("Getting RX humidity: %.2f %%", rx_humidity);
    return rx_humidity;
}

void print_sensor_data(void)
{
    static uint8_t data_buffer[APP_RX_BUFFER_SIZE];

    if(!app_rx_buffer_available()) {
        // LOG_WRN("No data available from RX buffer");
        k_msleep(1);
        return;
    }
    // LOG_INF("Data available from RX buffer (dropped=%d pool_exhausted=%d)", rx_fifo_dropped_count(), rx_pool_exhausted_count());
    int err = app_rx_buffer_read(data_buffer, sizeof(data_buffer));
    if (err < 0) {
        LOG_INF("No data available from RX buffer: %d", err);
        k_msleep(100);
        return;
    }

    if (err > 0) {
        float temp, hum;
        memcpy(&temp, &data_buffer[0], sizeof(float));
        memcpy(&hum, &data_buffer[4], sizeof(float));
        printk("Received Sensor Data - Temp: %.2f C, Hum: %.2f %%", temp, hum);
        // LOG_WRN("Received Sensor Data - Temp: %.2f C, Hum: %.2f %%", temp, hum);
        printk("\n");
        rx_temperature = temp;
        rx_humidity = hum;
    }
}
