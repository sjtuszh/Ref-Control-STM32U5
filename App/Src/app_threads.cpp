#include "app_threads.hpp"
#include "app_data.hpp"
#include "app_display.hpp"
#include "app_led.hpp"
#include "app_sensor.hpp"
#include "tx_api.h"

void OledThreadMain(void) {
    AppDisplayInit();

    while (1) {
        AppDisplayShowTscScreen();
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}

void LedThreadMain(void) {
    while (1) {
        AppLedUpdate();
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}

void SensorThreadMain(void) {
    AppSensorInit();

    while (1) {
        AppSensorUpdate();
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 20);
    }
}
