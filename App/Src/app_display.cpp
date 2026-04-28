#include "app_display.hpp"
#include "app_data.hpp"
#include "SSD1306_SIMPLE.hpp"

namespace {

static SSD1306_SIMPLE g_oled;

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

void PrintLabelValue(uint8_t x, uint8_t y, const char* label, uint32_t value) {
    char value_buffer[11];

    UIntToString(value, value_buffer, sizeof(value_buffer));

    g_oled.setCursor(x, y);
    g_oled.print(label);
    g_oled.print(value_buffer);
}

} // namespace

void AppDisplayInit() {
    g_oled.init();
    g_oled.setContrast(0xFF);
}

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

void AppDisplayShowTscScreen() {
    AppTscSnapshot snapshot = {};
    AppDataGetTscSnapshot(&snapshot);

    g_oled.clear();

    for (uint32_t i = 0; i < APP_TSC_CHANNEL_COUNT; ++i) {
        g_oled.setCursor(0, static_cast<uint8_t>(i * 8U));
        g_oled.printf("%lu:%4lu %c",
                      static_cast<unsigned long>(i + 1U),
                      static_cast<unsigned long>(snapshot.channels[i].raw),
                      (snapshot.channels[i].touched != 0U) ? '*' : '-');
    }

    g_oled.update();
}
