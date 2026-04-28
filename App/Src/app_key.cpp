#include "app_key.hpp"
#include "app_comm.hpp"
#include "main.h"
#include <stdint.h>

namespace {

struct KeyState {
    const char* name;
    GPIO_TypeDef* port;
    uint16_t pin;
    uint8_t stable_state;
    uint8_t last_sample;
    uint8_t debounce_count;
};

constexpr uint8_t APP_KEY_DEBOUNCE_COUNT = 3U;

KeyState g_keys[] = {
    {"SW2", SW2_GPIO_Port, SW2_Pin, 1U, 1U, 0U},
    {"SW3", SW3_GPIO_Port, SW3_Pin, 1U, 1U, 0U},
    {"SW4", SW4_GPIO_Port, SW4_Pin, 1U, 1U, 0U},
};

uint8_t ReadKeyLevel(const KeyState& key) {
    return (HAL_GPIO_ReadPin(key.port, key.pin) == GPIO_PIN_SET) ? 1U : 0U;
}

} // namespace

void AppKeyInit(void) {
    for (auto& key : g_keys) {
        const uint8_t level = ReadKeyLevel(key);
        key.stable_state = level;
        key.last_sample = level;
        key.debounce_count = 0U;
    }
}

void AppKeyUpdate(void) {
    for (auto& key : g_keys) {
        const uint8_t sample = ReadKeyLevel(key);

        if (sample == key.last_sample) {
            if (key.debounce_count < APP_KEY_DEBOUNCE_COUNT) {
                key.debounce_count++;
            }
        } else {
            key.last_sample = sample;
            key.debounce_count = 0U;
        }

        if ((key.debounce_count >= APP_KEY_DEBOUNCE_COUNT) && (sample != key.stable_state)) {
            key.stable_state = sample;
            AppUsbLogPrintf("[KEY] %s %s\r\n", key.name, (sample == 0U) ? "PRESSED" : "RELEASED");
        }
    }
}
