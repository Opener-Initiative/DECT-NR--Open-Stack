#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include <stddef.h>
#include <stdint.h>
#include <zephyr/device.h>

void uart_init(void);

/* callback que corresponde a la IRQ del dispositivo UART (se exporta para Zephyr) */
void esp_response_cb(const struct device *dev, void *user_data);

#endif
