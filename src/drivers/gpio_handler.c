#include "gpio_handler.h"
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(gpio_h);

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

void gpio_init(void)
{
    if (device_is_ready(led.port)) {
        gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
        LOG_INF("Configured LED pin as output");
    }
    else
    {
        LOG_ERR("Failed to configure LED pin");
    }
    if (device_is_ready(led1.port)) {
        gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
        LOG_INF("Configured LED1 pin as output");
    }
    else
    {
        LOG_ERR("Failed to configure LED1 pin");
    }
    
    int err = dk_leds_init();
    if (err) {
        LOG_ERR("Failed to initialize LED driver");
    }

}

void gpio_set_led(int led_num, bool state)
{
    // if (led_num == 0 && device_is_ready(led.port)) {
    //     gpio_pin_set_dt(&led, state ? 1 : 0);
    // } else if (led_num == 1 && device_is_ready(led1.port)) {
    //     gpio_pin_set_dt(&led1, state ? 1 : 0);
    // }
    switch (led_num)
    {
    case 0:
        if(state)
        {
           dk_set_led_on(DK_LED1);
        }
        else
        {
            dk_set_led_off(DK_LED1);
        }
        
    case 1:
        if (state)
        {
            dk_set_led_on(DK_LED2);
        }
        else
        {
            dk_set_led_off(DK_LED2);
        }
    
    default:
        break;
    }
}
