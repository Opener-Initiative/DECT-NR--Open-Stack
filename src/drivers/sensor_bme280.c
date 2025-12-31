#include "sensor_bme280.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bme280, LOG_LEVEL_INF);

static const struct device *bme280_dev;

int sensor_bme280_init(void)
{
    bme280_dev = DEVICE_DT_GET_ANY(bosch_bme280);
    if (!bme280_dev) {
        LOG_ERR("No se encontró el dispositivo BME280 en el DT");
        return -ENODEV;
    }
    if (!device_is_ready(bme280_dev)) {
        LOG_ERR("El dispositivo BME280 no está listo");
        return -EIO;
    }
    LOG_INF("BME280 inicializado correctamente");
    return 0;
}

int sensor_bme280_read(struct bme280_data *data)
{
    struct sensor_value temp, hum, press;

    if (!bme280_dev) return -ENODEV;

    int ret = sensor_sample_fetch(bme280_dev);
    if (ret < 0) return ret;

    sensor_channel_get(bme280_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    sensor_channel_get(bme280_dev, SENSOR_CHAN_HUMIDITY, &hum);
    sensor_channel_get(bme280_dev, SENSOR_CHAN_PRESS, &press);

    data->temperature = sensor_value_to_double(&temp);
    data->humidity    = sensor_value_to_double(&hum);
    data->pressure    = sensor_value_to_double(&press);

    LOG_INF("BME280: Temp=%.2f °C, Hum=%.2f %%, Press=%.2f kPa",
            data->temperature, data->humidity, data->pressure / 1000);

    return 0;
}

double get_bme280_temperature(void)
{
    struct bme280_data data;
    if (sensor_bme280_read(&data) == 0) {
        return data.temperature;
    }
    return -273.15; // Valor inválido
}

double get_bme280_humidity(void)
{
    struct bme280_data data;
    if (sensor_bme280_read(&data) == 0) {
        return data.humidity;
    }
    return -1; // Valor inválido
}

double get_bme280_pressure(void)
{
    struct bme280_data data;
    if (sensor_bme280_read(&data) == 0) {
        return data.pressure;
    }
    return -1; // Valor inválido
}