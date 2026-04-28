#include "app_threads.hpp"
#include "app_data.hpp"
#include "app_display.hpp"
#include "app_key.hpp"
#include "app_led.hpp"
#include "app_comm.hpp"
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
    AppCommInit();

    uint32_t report_divider = 0U;

    while (1) {
        AppSensorUpdate();

        if (++report_divider >= 20U) {
            report_divider = 0U;
            AppUsbLogPrintf("[TSC] scan running\r\n");
        }

        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 20);
    }
}

void KeyThreadMain(void) {
    AppCommInit();
    AppKeyInit();
    AppUsbLogPrintf("[KEY] thread started\r\n");

    while (1) {
        AppKeyUpdate();
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 100);
    }
}
