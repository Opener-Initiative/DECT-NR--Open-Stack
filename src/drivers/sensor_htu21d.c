#include "sensor_htu21d.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(htu21d, LOG_LEVEL_INF);

//// Using I2C controller directly to communicate with HTU21D address 0x40

static const struct device *i2c_dev;
static const uint16_t htu_addr = 0x40;

int sensor_htu21d_init(void)
{
    //// Returning values of this methods do not indicate if the sensor is present or not. 
    //// Using i2c2 controller. To use other controller modify devicetree

    i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c2));
    if (!device_is_ready(i2c_dev)) {
        LOG_WRN("I2C controller 'i2c2' not ready or does not exist");
        return -ENODEV;
    }

    LOG_INF("I2C controller 'i2c2' ready, using addr 0x%02x", htu_addr);
    LOG_INF("HTU21D I2C initialized (bus i2c2)");

    if(sensor_htu21d_is_ready())
        LOG_INF("HTU21D sensor detected and ready");
    else
    {
        LOG_WRN("HTU21D sensor not detected or not ready");
        return -1;
    }
    return 0;
}

bool sensor_htu21d_is_ready(void)
{
    static struct htu21d_data data;
    if(sensor_htu21d_read(&data) < 0) return false;
    return true;
}

int sensor_htu21d_read(struct htu21d_data *data)
{
    if (!data) return -EINVAL;
    if (!i2c_dev) return -ENODEV;

    int ret;
    uint8_t buf[3];

    /* Read temperature: command 0xE3 (no hold master) */
    uint8_t cmd = 0xE3;
    ret = i2c_write_read(i2c_dev, htu_addr, &cmd, 1, buf, 3);
    if (ret < 0) {
        LOG_ERR("Error reading temperature HTU21D: %d", ret);
        return ret;
    }
    uint16_t raw_temp = ((uint16_t)buf[0] << 8) | buf[1];
    raw_temp &= 0xFFFC; /* clear status bits */
    data->temperature = -46.85f + 175.72f * (float)raw_temp / 65536.0f;

    /* Read humidity: command 0xE5 (no hold master) */
    cmd = 0xE5;
    ret = i2c_write_read(i2c_dev, htu_addr, &cmd, 1, buf, 3);
    if (ret < 0) {
        LOG_ERR("Error reading humidity HTU21D: %d", ret);
        return ret;
    }
    uint16_t raw_hum = ((uint16_t)buf[0] << 8) | buf[1];
    raw_hum &= 0xFFFC;
    data->humidity = -6.0f + 125.0f * (float)raw_hum / 65536.0f;

    // LOG_INF("HTU21D: Temp=%.2f °C, Hum=%.2f %%", data->temperature, data->humidity);

    return 0;
}

double get_htu21d_temperature(void)
{
    struct htu21d_data data;
    if (sensor_htu21d_read(&data) == 0) {
        return data.temperature;
    }
    return -273.15;
}

double get_htu21d_humidity(void)
{
    struct htu21d_data data;
    if (sensor_htu21d_read(&data) == 0) {
        return data.humidity;
    }
    return -1;
}
