#include "app_comm.hpp"
#include "ux_device_cdc_acm.h"
#include "tx_api.h"
#include <cstddef>
#include <stdarg.h>

namespace {

TX_MUTEX g_usb_log_mutex;
bool g_usb_log_initialized = false;
char g_log_buffer[128];

size_t AppendChar(char* buffer, size_t buffer_size, size_t index, char ch) {
    if (index + 1U < buffer_size) {
        buffer[index] = ch;
    }
    return index + 1U;
}

size_t AppendString(char* buffer, size_t buffer_size, size_t index, const char* str) {
    const char* text = (str != nullptr) ? str : "(null)";
    while (*text != '\0') {
        index = AppendChar(buffer, buffer_size, index, *text++);
    }
    return index;
}

size_t AppendUnsignedLong(char* buffer, size_t buffer_size, size_t index, unsigned long value) {
    char temp[10];
    size_t digit_count = 0U;

    do {
        temp[digit_count++] = static_cast<char>('0' + (value % 10UL));
        value /= 10UL;
    } while (value != 0UL && digit_count < sizeof(temp));

    while (digit_count > 0U) {
        index = AppendChar(buffer, buffer_size, index, temp[--digit_count]);
    }

    return index;
}

size_t AppendSignedLong(char* buffer, size_t buffer_size, size_t index, long value) {
    unsigned long magnitude;

    if (value < 0) {
        index = AppendChar(buffer, buffer_size, index, '-');
        magnitude = static_cast<unsigned long>(-(value + 1L)) + 1UL;
    } else {
        magnitude = static_cast<unsigned long>(value);
    }

    return AppendUnsignedLong(buffer, buffer_size, index, magnitude);
}

int FormatToBuffer(char* buffer, size_t buffer_size, const char* format, va_list args) {
    size_t index = 0U;

    if (buffer == nullptr || buffer_size == 0U || format == nullptr) {
        return -1;
    }

    while (*format != '\0') {
        if (*format != '%') {
            index = AppendChar(buffer, buffer_size, index, *format++);
            continue;
        }

        ++format;
        bool long_flag = false;
        if (*format == 'l') {
            long_flag = true;
            ++format;
        }

        switch (*format) {
            case '\0':
                --format;
                break;
            case '%':
                index = AppendChar(buffer, buffer_size, index, '%');
                break;
            case 'c':
                index = AppendChar(buffer, buffer_size, index, static_cast<char>(va_arg(args, int)));
                break;
            case 's':
                index = AppendString(buffer, buffer_size, index, va_arg(args, const char*));
                break;
            case 'd':
            case 'i':
                if (long_flag) {
                    index = AppendSignedLong(buffer, buffer_size, index, va_arg(args, long));
                } else {
                    index = AppendSignedLong(buffer, buffer_size, index, va_arg(args, int));
                }
                break;
            case 'u':
                if (long_flag) {
                    index = AppendUnsignedLong(buffer, buffer_size, index, va_arg(args, unsigned long));
                } else {
                    index = AppendUnsignedLong(buffer, buffer_size, index, va_arg(args, unsigned int));
                }
                break;
            default:
                index = AppendChar(buffer, buffer_size, index, '%');
                if (long_flag) {
                    index = AppendChar(buffer, buffer_size, index, 'l');
                }
                index = AppendChar(buffer, buffer_size, index, *format);
                break;
        }

        if (*format != '\0') {
            ++format;
        }
    }

    const size_t terminator_index = (index < buffer_size) ? index : (buffer_size - 1U);
    buffer[terminator_index] = '\0';
    return static_cast<int>(index);
}

} // namespace

void AppCommInit(void) {
    if (g_usb_log_initialized) {
        return;
    }

    if (tx_mutex_create(&g_usb_log_mutex, const_cast<char*>("usb_log_mutex"), TX_NO_INHERIT) == TX_SUCCESS) {
        g_usb_log_initialized = true;
    }
}

uint8_t AppUsbCdcIsReady(void) {
    return UsbCdcAcmIsReady();
}

void AppUsbCdcWrite(const char* text) {
    if (!g_usb_log_initialized || text == nullptr) {
        return;
    }

    if (tx_mutex_get(&g_usb_log_mutex, TX_WAIT_FOREVER) != TX_SUCCESS) {
        return;
    }

    if (UsbCdcAcmIsReady()) {
        UsbCdcAcmWrite(text);
    }

    tx_mutex_put(&g_usb_log_mutex);
}

void AppUsbLogPrintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    (void)FormatToBuffer(g_log_buffer, sizeof(g_log_buffer), format, args);
    va_end(args);

    AppUsbCdcWrite(g_log_buffer);
}
