#ifndef SENSOR_HTU21D_H
#define SENSOR_HTU21D_H

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <stdint.h>
#include <stdbool.h>

struct htu21d_data {
    float temperature;
    float humidity;
};

/// @brief Initialize the HTU21D sensor
int sensor_htu21d_init(void);

/// @brief Check if the HTU21D sensor is ready
bool sensor_htu21d_is_ready(void);

/// @brief Read data from the HTU21D sensor
/// @param data Pointer to the structure to store the read data
/// @return 0 on success, negative error code on failure
int sensor_htu21d_read(struct htu21d_data *data);

//// Data obtaining methods
double get_htu21d_temperature(void);
double get_htu21d_humidity(void);

#endif
