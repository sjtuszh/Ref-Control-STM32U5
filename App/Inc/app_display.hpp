#ifndef REF_CONTROL_STM32U5_APP_DISPLAY_HPP
#define REF_CONTROL_STM32U5_APP_DISPLAY_HPP

#include <cstdint>

void AppDisplayInit();
void AppDisplayShowTestScreen(uint32_t tick_count, uint32_t seconds);
void AppDisplayShowTscScreen();

#endif // REF_CONTROL_STM32U5_APP_DISPLAY_HPP
