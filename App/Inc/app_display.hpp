// ####### 显示业务头文件 #######
// 这里放 OLED 相关的业务接口，后续菜单、状态页、告警页都可以继续加在这里。

#ifndef REF_CONTROL_STM32U5_APP_DISPLAY_HPP
#define REF_CONTROL_STM32U5_APP_DISPLAY_HPP

#include <cstdint>

void AppDisplayInit();
void AppDisplayShowTestScreen(uint32_t tick_count, uint32_t seconds);

#endif // REF_CONTROL_STM32U5_APP_DISPLAY_HPP
