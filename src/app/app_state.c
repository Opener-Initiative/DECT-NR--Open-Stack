#include "app_state.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../drivers/modem_handler.h"
#include "../protocol/procedures.h"
#include "../common/utils.h"
#include "../config/device_config.h"
#include "../protocol/procedures.h"
#include "../common/dect_timers.h"
#include "../drivers/gpio_handler.h"
#include "../common/network.h"
#include "../fsm/fsm.h"
#include "../drivers/sensor_htu21d.h"
#include "../fsm/fsm_event.h"

LOG_MODULE_DECLARE(app);

/// @brief Local variables
static int STATE = SEND_BEACON;
static int NEXT_STATE = WAIT_ASSOC_REQ;
static struct TXParams tp;
static struct DataMessage dm;
//// associated y isFT están definidas en network.c como variable global


//// @brief Initialize the application state, structures, timers, and global variables.
void app_init(void)
{
    //// Initialize Network structures
    network_init();
    rx_consumer_init();

    //// Initialize timer system
    timers_init();

    if (USING_GATEWAYS && IS_GATEWAY) {
        NEXT_STATE = SEND_BEACON;
    }
    else if (USING_GATEWAYS && !IS_GATEWAY) {
        NEXT_STATE = WAIT_BEACON;   
        timers_start(ISOLATION_TIMER, K_SECONDS(10), 1);
    }
    else if (!USING_GATEWAYS)
    {
        int randomDelay = generateRandomNumber(5000);
        LOG_INF("Set beacon timer to %d", 2000 + randomDelay);
        timers_start(BEACON_TIMER, K_MSEC(2000 + randomDelay), 2);
        NEXT_STATE = WAIT_BEACON;
    }


    gpio_set_led(0, true);  
    k_msleep(500);
    gpio_set_led(0, false); 
    k_msleep(500);
    gpio_set_led(0, true);  
    k_msleep(500);
    gpio_set_led(0, false); 

    LOG_INF("App initialized");
}


/// @brief 
/// @param data 
/// @param len 
void app_on_uart_data(const uint8_t *data, size_t len)
{
    size_t copy_len = len > sizeof(dm.payload) ? sizeof(dm.payload) : len;
    memcpy(dm.payload, data, copy_len);

    txData(&dm, &tp);

    modem_tx(dm.payload, sizeof(dm.payload), dm.phyheader, 5);

    LOG_INF("app_on_uart_data: enviado paquete (len=%d)", (int)copy_len);
}

int app_get_state(void)
{
    return STATE;
}

int app_get_next_state(void)
{
    return NEXT_STATE;
}

void app_set_next_state(int next)
{
    NEXT_STATE = next;
}



/// @brief Main application loop. Blocking.
void app_run(void)
{
    while (1) {

        struct fsm_event evt;

        if (fsm_event_get(&evt, K_MSEC(50)) == 0) {
            switch (evt.type) {
                case FSM_EVENT_PACKET_RX:
                    process_rx_packet(evt.rx.data, evt.rx.len);
                    break;
                case FSM_EVENT_TIMER:
                    app_on_timer_event(&evt.timer);
                    break;
                default:
                    break;
            }
        }
        // LOG_INF("STATE: %d, NEXT_STATE: %d", STATE, NEXT_STATE);
        STATE = NEXT_STATE;

        switch (STATE) {
            case SEND_BEACON:
                LOG_INF("Sending Beacon");
                sendBeacon();

                NEXT_STATE = WAIT_ASSOC_REQ;
                break;

            case SEND_ASSOC_REQ:
                LOG_INF("Sending Association Request");
                sendAssocReq();

                NEXT_STATE = WAIT_ASSOC_RESP;
                break;

            case SEND_ASSOC_RESP:
                LOG_INF("Sending Association Response");
                sendAssocResp();

                // NEXT_STATE = WAIT_RX;
                break;

            case SEND_DATA:
                LOG_INF("Sending Data");
                sendData();

                NEXT_STATE = FT_OPERATIONS;
                break;

            case FT_OPERATIONS:
                LOG_INF("FT Operations");

                NEXT_STATE = ftOperations();
                break;

            case SEND_KAIE:
                LOG_INF("Sending Keep Alive IE");
                // sendKAIE();

                NEXT_STATE = WAIT_RX;
                break;

            case SEND_METRIC_REQ:
                LOG_INF("Sending Metrics Request");
                // sendMetricReq();

                NEXT_STATE = WAIT_METRIC_RESP;
                break;

            case SEND_METRIC_RESP:
                LOG_INF("Sending Metrics Response");
                // sendMetricResp();

                NEXT_STATE = WAIT_RX;
                break;




            case WAIT_BEACON:
                LOG_INF("Waiting for Beacon");                
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_ASSOC_REQ:
                LOG_INF("Waiting for Association Request");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_ASSOC_RESP:
                LOG_INF("Waiting for Association Response");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_DATA:
                LOG_INF("Waiting for Data");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_RX:
                LOG_INF("Waiting for any incoming message");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_KAIE:
                LOG_INF("Waiting for Keep Alive IE");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_METRIC_REQ:
                LOG_INF("Waiting for Metrics Request");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;

            case WAIT_METRIC_RESP:
                LOG_INF("Waiting for Metrics Response");
                modem_rx(NRF_MODEM_DECT_PHY_RX_MODE_SINGLE_SHOT, 1);
                break;



            default:
                k_msleep(100);
                break;
        }

        k_msleep(10);
    }
}

void app_on_timer_event(const struct timer_event *evt)
{
    switch (evt->type) {
        case BEACON_TIMER:
            LOG_INF("Beacon timer expired");
            if(!USING_GATEWAYS)
            {
                int randomDelay = generateRandomNumber(5000);
                LOG_INF("Set beacon timer to %d", 2000 + randomDelay);
                timers_start(BEACON_TIMER, K_MSEC(2000 + randomDelay), 2);
            }
            else
            {
                LOG_INF("Set beacon timer to %d", BEACON_PERIOD);
                timers_start(BEACON_TIMER, K_MSEC(BEACON_PERIOD), 2);
            }
            
            NEXT_STATE = SEND_BEACON;
            break;

        case ASSOC_MSG_TIMER:
            LOG_INF("Assoc Message timer expired");

            //// Disabled filter in RX
            disable_rx_filter();

            //// Delete last registered network, the one trying to associate with
            deleteLastNetwork();

            // Rutina de no aislamiento
            NEXT_STATE = WAIT_BEACON;

            if (!is_associated() && !is_isFT()) {
                if(RING_LEVEL == 2)
                {
                    int randomDelay = generateRandomNumber(5000);
                    LOG_INF("Set beacon timer to %d", 2000 + randomDelay);
                    timers_start(BEACON_TIMER, K_MSEC(2000 + randomDelay), 2);
                }
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


    while(1)
    {
        k_msleep(1000);

        LOG_INF("Temperatura: %.2f °C", get_htu21d_temperature());
        LOG_INF("Humedad: %.2f %%", get_htu21d_humidity());
        k_msleep(1000);

    }
}
