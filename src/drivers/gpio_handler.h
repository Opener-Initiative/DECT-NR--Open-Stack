#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H
#include <stdbool.h>
void gpio_init(void);
void gpio_set_led(int led_num, bool state);
#endif
