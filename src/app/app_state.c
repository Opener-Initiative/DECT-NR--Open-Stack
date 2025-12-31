#include "app_state.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../drivers/modem_handler.h"
#include "../protocol/procedures.h"
#include "../drivers/uart_handler.h"
#include "../common/utils.h"
#include "../config/device_config.h"
#include "../protocol/procedures.h"
#include "../common/dect_timers.h"
#include "../drivers/gpio_handler.h"
#include "../common/network.h"
#include "../drivers/sensor_htu21d.h"
#include "../fsm/fsm_event.h"
#include "../drivers/display_handler.h"
#include "../common/data_buffers.h"
#include "../common/rx_fifo.h"
#include "../common/tx_fifo.h"
#include "../common/modem_operator.h"

LOG_MODULE_DECLARE(app);

/// @brief Local variables
static int STATE = ASSOC_WIN_STATE;
static int NEXT_STATE = ASSOC_WIN_STATE;
static bool FT_OP = false;
static struct TXParams tp;
static struct DataMessage dm;
static bool ftOperationsDone = false;
//// associated y isFT están definidas en network.c como variable global

void app_ftOperationsDone_set(bool state) {ftOperationsDone = state;}
bool app_ftOperationsDone_get(void) {return ftOperationsDone;}


//// @brief Initialize the application state, structures, timers, and global variables.
void app_init(void)
{
    //// Initialize Network structures
    rx_fifo_pool_init();  // Initialize static packet pool before consumer
    tx_fifo_pool_init();  // Initialize TX packet pool used by packet_generator

    //// Initialize timer system
    timers_init();

    if(IS_GATEWAY) FT_OP = true;
    else FT_OP = false;
    timers_start(ASSOC_WIN, K_SECONDS(1), 1);




    // DK LEDs blink to indicate initialization completed
    for (int i = 0; i < 2; i++) {
        gpio_set_led(0, true);
        k_msleep(200);
        gpio_set_led(0, false);
        k_msleep(200);
    }

    LOG_INF("App initialized");
}


/// @brief 
/// @param data 
/// @param len 
void app_on_uart_data(const uint8_t *data, size_t len)
{
    /* ejemplo simple: empaquetar y enviar como DataMessage */
    size_t copy_len = len > sizeof(dm.payload) ? sizeof(dm.payload) : len;
    memcpy(dm.payload, data, copy_len);

    /* llama a la función que genera la PDU (procedures.c) */
    txData(&dm, &tp);

    /* y manda por modem (usa wrapper modem_tx) */
    modem_tx(dm.payload, sizeof(dm.payload), dm.phyheader, 5, 0);

    LOG_INF("app_on_uart_data: enviado paquete (len=%d)", (int)copy_len);
}

int app_get_state(void){ return STATE; }

int app_get_next_state(void){ return NEXT_STATE; }

void app_set_next_state(int next){ NEXT_STATE = next; }
    



/// @brief Main application loop. Blocking.
void app_run(void)
{
    struct fsm_event evt;
    
    while (1) 
    {
        while (fsm_event_get(&evt, K_MSEC(50)) == 0) {
            switch (evt.type) {
                case FSM_EVENT_PACKET_RX:
                    process_rx_packet(evt.rx.data, evt.rx.len, evt.rx.transmitter_srdid);
                    break;
                case FSM_EVENT_TIMER:
                    app_on_timer_event(&evt.timer);
                    break;
                default:
                    break;
            }
        }

        switch(STATE)
        {
            case ASSOC_WIN_STATE:
                break;

            case DATA_WIN_STATE:
                break;
        }
        k_msleep(1);
    }
}

void app_on_timer_event(const struct timer_event *evt)
{
    switch (evt->type) {
        case ASSOC_WIN:
            LOG_INF("Association Window timer expired");
            if(ENABLE_ASSOCIATION)
            {
                if(!is_associated()&& IS_GATEWAY)
                {
                    LOG_INF("Device not associated, returning to beacon transmission");
                    NEXT_STATE = ASSOC_WIN_STATE;
                    timers_start(ASSOC_WIN, K_MSEC(BEACON_PERIOD+100), 1);

                    //// Send beacon
                    sendBeacon();
                    modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_CONTINUOUS, BEACON_PERIOD);
                }
                else if(is_associated() && IS_GATEWAY)
                {
                    LOG_INF("Device associated, proceeding to data window");
                    timers_start(DATA_WIN, K_MSEC(BROADCAST_IND_PERIOD), 1);
                    NEXT_STATE = DATA_WIN_STATE;

                    //// Send Broadcast Indication
                    sendBroadcastIndication();
                    modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_CONTINUOUS, BROADCAST_IND_PERIOD);
                }
                else if(is_associated() && !IS_GATEWAY)
                {
                    NEXT_STATE = DATA_WIN_STATE;
                    timers_start(DATA_WIN, K_SECONDS(5), 1);
                    modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, BROADCAST_IND_PERIOD);
                }
                else
                {
                    LOG_INF("Device not associated, waiting to associate");
                    modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, BEACON_PERIOD+generateRandomNumber(1000));
                    timers_start(ASSOC_WIN, K_MSEC(BEACON_PERIOD+1000), 1);
                }

            }
            else
            {
                LOG_INF("Association disabled, proceeding to data window");
                NEXT_STATE = DATA_WIN_STATE;
                timers_start(DATA_WIN, K_SECONDS(5), 1);
            }
            break;

        case DATA_WIN:
            LOG_INF("Data Window timer expired");
            if(ENABLE_ASSOCIATION)
            {
                if(IS_GATEWAY)
                {
                    NEXT_STATE = ASSOC_WIN_STATE;
                    timers_start(ASSOC_WIN, K_MSEC(BEACON_PERIOD+100), 1);

                    //// Send beacon
                    sendBeacon();
                    modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_CONTINUOUS, BEACON_PERIOD);
                }
                else
                {
                    NEXT_STATE = ASSOC_WIN_STATE;
                    timers_start(ASSOC_WIN, K_MSEC(BEACON_PERIOD+100), 1);

                    modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, BEACON_PERIOD);
                }
            }
            else
            {
                timers_start(DATA_WIN, K_MSEC(BROADCAST_IND_PERIOD), 1);
                NEXT_STATE = DATA_WIN_STATE;

                //// Send Broadcast Indication
                // sendBroadcastIndication();
                // modem_operator_request_rx(NRF_MODEM_DECT_PHY_RX_MODE_CONTINUOUS, BROADCAST_IND_PERIOD);
                sendData();
            }
            break;

        default:
            LOG_INF("Unknown timer event");
            break;
    }
}

void app_test(void)
{
    LOG_INF("App test function called");
}

void app()
{
    gpio_init();
    uart_init();
    network_init();
    modem_init();

    app_init();
    LOG_INF("Starting main application loop");

    // app_test();

    app_run();
}