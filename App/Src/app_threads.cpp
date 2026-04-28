// ####### 应用线程实现文件 #######
// 这里负责组织各个线程“做什么”，但不负责 ThreadX 线程对象的创建。
// ThreadX 对象创建统一放在 Core/Src/app_threadx.c。

#include "app_threads.hpp"
#include "app_display.hpp"
#include "app_led.hpp"
#include "tx_api.h"

// ####### OLED 刷新线程 #######
// 专门负责屏幕初始化与周期刷新。
void OledThreadMain(void) {
    AppDisplayInit();

    uint32_t seconds = 0;

    while (1) {
        const uint32_t tick_count = tx_time_get();
        AppDisplayShowTestScreen(tick_count, seconds);

        ++seconds;
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}

// ####### LED 状态线程 #######
// 专门负责 LED 指示灯刷新。
void LedThreadMain(void) {
    while (1) {
        AppLedUpdate();
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);
    }
}
