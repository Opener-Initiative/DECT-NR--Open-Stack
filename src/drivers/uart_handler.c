#include "uart_handler.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#include "../app/app_state.h"
#include "../common/utils.h"

LOG_MODULE_REGISTER(uart_h);



#define ESP32_UART_NODE DT_ALIAS(esp32_uart)        /* defined in your overlay */
#define PC_UART_NODE    DT_CHOSEN(zephyr_shell_uart) /* console/logs */
#define MSG_SIZE (693*2)
#define ESP32_BUF_SIZE 700
#define UART_DEV "UART_3"
K_MSGQ_DEFINE(esp32_msgq, sizeof(uint8_t), MSG_SIZE, 1);
K_MSGQ_DEFINE(pc_msgq, sizeof(uint8_t), MSG_SIZE, 1);


static const struct device *esp32_dev;
static const struct device *pc_uart_dev;

static struct k_work esp32_work;
static struct k_work pc_work;

static uint8_t esp32_reception_buffer[700];
static size_t esp32_head = 0;   // write position
static size_t esp32_count = 0;  // number of valid bytes in the buffer

/// @brief Store a byte in the ESP32 circular buffer
static inline void esp32_circbuf_put(uint8_t byte)
{
    esp32_reception_buffer[esp32_head] = byte;
    esp32_head = (esp32_head + 1) % ESP32_BUF_SIZE;
    if (esp32_count < ESP32_BUF_SIZE) {
        esp32_count++;
    }
}

/// @brief Retrieve the last `max_len` bytes received from the ESP32
/// @param out_buf destination buffer
/// @param max_len maximum destination size
/// @return number of bytes copied
size_t uart_get_last_data(uint8_t *out_buf, size_t max_len)
{
    size_t to_copy = (esp32_count < max_len) ? esp32_count : max_len;

    // start read position (last to_copy bytes)
    size_t start = (esp32_head + ESP32_BUF_SIZE - to_copy) % ESP32_BUF_SIZE;

    for (size_t i = 0; i < to_copy; i++) {
        out_buf[i] = esp32_reception_buffer[(start + i) % ESP32_BUF_SIZE];
    }

    return to_copy;
}

/// @brief Handler to process ESP32 UART data outside interrupt context
/// @param work 
static void esp32_work_handler(struct k_work *work)
{
    uint8_t byte;
    uint8_t buffer[700];
    size_t pos = 0;

    while (k_msgq_get(&esp32_msgq, &byte, K_NO_WAIT) == 0) {
        if (pos < sizeof(buffer)) {
            buffer[pos++] = byte;
            if (byte == '\n') {
                /* Forward packet to the app */
                app_on_uart_data(buffer, pos);
                pos = 0;
            }
        } else {
            pos = 0; 
        }
    }
}

/// @brief Handler to process PC UART data outside interrupt context
/// @param work
static void pc_work_handler(struct k_work *work)
{
    uint8_t byte;
    uint8_t buffer[700];
    size_t pos = 0;

    while (k_msgq_get(&pc_msgq, &byte, K_NO_WAIT) == 0) {
        if (pos < sizeof(buffer)) {
            buffer[pos++] = byte;
            if (byte == '\n') {
                /* Decide what to do with data coming from the PC here */
                LOG_INF("PC UART: received packet len=%d", pos);
                // Example: forward to ESP32
                // uart_fifo_fill(esp32_dev, buffer, pos);
                pos = 0;
            }
        } else {
            pos = 0;
        }
    }
}

/// @brief Callback for ESP32 UART interrupts
/// @param dev 
/// @param user_data 
static void esp32_cb(const struct device *dev, void *user_data)
{
    uint8_t byte;
    if (!uart_irq_update(dev)) return;

    while (uart_irq_rx_ready(dev)) {
        if (uart_fifo_read(dev, &byte, 1) > 0) {
            k_msgq_put(&esp32_msgq, &byte, K_NO_WAIT);
            esp32_circbuf_put(byte);   // <-- Always store in the circular buffer
        }
    }

    if (!k_work_is_pending(&esp32_work)) {
        k_work_submit(&esp32_work);
    }
}

/// @brief Callback for PC UART interrupts
/// @param dev 
/// @param user_data 
static void pc_cb(const struct device *dev, void *user_data)
{
    uint8_t byte;
    if (!uart_irq_update(dev)) return;

    while (uart_irq_rx_ready(dev)) {
        if (uart_fifo_read(dev, &byte, 1) > 0) {
            k_msgq_put(&pc_msgq, &byte, K_NO_WAIT);
        }
    }

    if (!k_work_is_pending(&pc_work)) {
        k_work_submit(&pc_work);
    }
}

void uart_init(void)
{
    // ESP32 UART
    k_work_init(&esp32_work, esp32_work_handler);
    esp32_dev = device_get_binding(UART_DEV);
    if (!device_is_ready(esp32_dev)) {
        LOG_ERR("ESP32 UART not ready");
    } else {
        uart_irq_callback_user_data_set(esp32_dev, esp32_cb, NULL);
        uart_irq_rx_enable(esp32_dev);
        LOG_INF("ESP32 UART initialized");
    }

    /* PC UART */
    k_work_init(&pc_work, pc_work_handler);
    pc_uart_dev = DEVICE_DT_GET(PC_UART_NODE);
    if (!device_is_ready(pc_uart_dev)) {
        LOG_ERR("PC UART not ready");
    } else {
        uart_irq_callback_user_data_set(pc_uart_dev, pc_cb, NULL);
        uart_irq_rx_enable(pc_uart_dev);
        LOG_INF("PC UART initialized");
    }
}

/// @brief Send data to ESP32 via UART
void uart_send_to_esp32(const uint8_t *data, size_t len)
{
    if (!esp32_dev) return;
    uart_fifo_fill(esp32_dev, data, len);
}

/// @brief Send data to PC via UART
void uart_send_to_pc(const uint8_t *data, size_t len)
{
    if (!pc_uart_dev) return;
    uart_fifo_fill(pc_uart_dev, data, len);
}