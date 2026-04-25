//
// Created by 22320 on 2026/4/8.
//

#include "common_inc.h"
#include "main.h"
#include "tx_api.h"

void Main() {
    while (1) {
        HAL_GPIO_TogglePin(TEST_LED_GPIO_Port, TEST_LED_Pin);
        HAL_GPIO_TogglePin(TSC_LED1_GPIO_Port, TSC_LED1_Pin);
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}
