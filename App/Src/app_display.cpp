// ####### 显示业务实现文件 #######
// 这里集中放 OLED 显示相关逻辑，后续菜单、状态页、波形页、告警页都可以继续加在这里。

#include "app_display.hpp"
#include "SSD1306_SIMPLE.hpp"

namespace {

// ####### OLED 对象 #######
static SSD1306_SIMPLE g_oled;

// ####### 数字转字符串辅助函数 #######
void UIntToString(uint32_t value, char* buffer, size_t buffer_size) {
    if (buffer == nullptr || buffer_size == 0U) {
        return;
    }

    char temp[11];
    size_t index = 0;

    do {
        temp[index++] = static_cast<char>('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U && index < sizeof(temp));

    size_t out_index = 0;
    while (index > 0U && out_index + 1U < buffer_size) {
        buffer[out_index++] = temp[--index];
    }
    buffer[out_index] = '\0';
}

// ####### 标签数值显示辅助函数 #######
void PrintLabelValue(uint8_t x, uint8_t y, const char* label, uint32_t value) {
    char value_buffer[11];

    UIntToString(value, value_buffer, sizeof(value_buffer));

    g_oled.setCursor(x, y);
    g_oled.print(label);
    g_oled.print(value_buffer);
}

} // namespace

// ####### 显示初始化 #######
void AppDisplayInit() {
    g_oled.init();
    g_oled.setContrast(0xFF);
}

// ####### 测试界面显示 #######
void AppDisplayShowTestScreen(uint32_t tick_count, uint32_t seconds) {
    const uint8_t progress = static_cast<uint8_t>((seconds * 10U) % 101U);

    g_oled.clear();
    g_oled.setCursor(0, 0);
    g_oled.print("SSD1306 OK");
    PrintLabelValue(0, 16, "tick:", tick_count);
    PrintLabelValue(0, 32, "sec :", seconds);
    g_oled.drawProgressBar(0, 50, 128, 12, progress);
    g_oled.update();
}
