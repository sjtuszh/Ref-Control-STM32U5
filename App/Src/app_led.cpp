// ####### LED 业务实现文件 #######
// 这里集中放 LED 指示灯刷新逻辑，后续可以继续演进为状态机。

#include "app_led.hpp"
#include "main.h"

void AppLedUpdate() {
    HAL_GPIO_TogglePin(TEST_LED_GPIO_Port, TEST_LED_Pin);
    HAL_GPIO_TogglePin(TSC_LED1_GPIO_Port, TSC_LED1_Pin);
    HAL_GPIO_TogglePin(TSC_LED2_GPIO_Port, TSC_LED2_Pin);
}
