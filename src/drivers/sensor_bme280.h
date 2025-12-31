#ifndef SENSOR_BME280_H
#define SENSOR_BME280_H

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <stdint.h>
#include <stdbool.h>

struct bme280_data {
    float temperature;
    float humidity;
    float pressure;
};

/* Inicializa el sensor BME280 */
int sensor_bme280_init(void);

/* Lee datos del sensor BME280 */
int sensor_bme280_read(struct bme280_data *data);

#endif /* SENSOR_BME280_H */
