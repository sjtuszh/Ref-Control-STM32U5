// ssd1306_simple.cpp
// Created by 22320 on 2025/12/18.
// Modified to implement SSD1306_SIMPLE class with printf-like interface

#include "SSD1306_SIMPLE.hpp"
#include <cstring>

// ####### 默认字体选择区域 #######
// 根据 ssd1306_conf.h 中使能的字体宏，选择一个默认字体。
// 业务层如果没有手动 setDefaultFont()，就使用这里的默认值。
#ifdef SSD1306_INCLUDE_FONT_6x8
static const SSD1306_Font_t* DEFAULT_FONT_PTR = &Font_6x8;
#elif defined(SSD1306_INCLUDE_FONT_7x10)
static const SSD1306_Font_t* DEFAULT_FONT_PTR = &Font_7x10;
#elif defined(SSD1306_INCLUDE_FONT_11x18)
static const SSD1306_Font_t* DEFAULT_FONT_PTR = &Font_11x18;
#else
#error "No font defined in ssd1306_conf.h"
#endif

namespace {

// ####### 轻量字符串拼接辅助函数 #######
// 这一组函数用于替代标准库 snprintf/vsnprintf，
// 这样在 STM32 + ThreadX 场景下更稳定，也更容易控制资源占用。

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
    size_t digit_count = 0;

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

// ####### 轻量格式化核心 #######
// 负责解析简单格式串，并将结果写入目标缓冲区。
// 当前支持：
// %s %c %d %i %u %ld %li %lu %%
int FormatToBuffer(char* buffer, size_t buffer_size, const char* format, va_list args) {
    size_t index = 0;

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

    if (buffer_size > 0U) {
        const size_t terminator_index = (index < buffer_size) ? index : (buffer_size - 1U);
        buffer[terminator_index] = '\0';
    }

    return static_cast<int>(index);
}

} // namespace

// ####### 构造与基础控制实现 #######

SSD1306_SIMPLE::SSD1306_SIMPLE() : currentFont(DEFAULT_FONT_PTR) {
    // 构造时先清空内部格式化缓冲区，避免残留脏数据。
    formatBuffer[0] = '\0';
}

void SSD1306_SIMPLE::init() {
    // 调用底层 C 驱动完成 SSD1306 上电初始化。
    ssd1306_Init();
}

void SSD1306_SIMPLE::clear() {
    // 清空显存缓冲区，并把文字输出起点恢复到左上角。
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
}

void SSD1306_SIMPLE::update() {
    // 把当前显存缓冲区真正刷新到 OLED 面板。
    ssd1306_UpdateScreen();
}

void SSD1306_SIMPLE::setCursor(uint8_t x, uint8_t y) {
    // 设置后续文本绘制的起始坐标。
    ssd1306_SetCursor(x, y);
}

uint8_t SSD1306_SIMPLE::getCursorX() const {
    // 当前版本没有直接暴露底层内部状态，因此先返回占位值。
    return 0;
}

uint8_t SSD1306_SIMPLE::getCursorY() const {
    // 当前版本没有直接暴露底层内部状态，因此先返回占位值。
    return 0;
}

// ####### 文本输出实现 #######

void SSD1306_SIMPLE::print(const char* str) {
    // 使用当前默认字体输出字符串。
    print(str, currentFont);
}

void SSD1306_SIMPLE::println(const char* str) {
    // 先输出文本。
    print(str);
    // 这里暂未实现严格的自动换行，仅保留接口形式，便于后续扩展。
}

int SSD1306_SIMPLE::printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(currentFont, format, args);
    va_end(args);
    return result;
}

void SSD1306_SIMPLE::print(const char* str, const SSD1306_Font_t* font) {
    // 直接调用底层字符串绘制函数，把文本写入 OLED 缓冲区。
    ssd1306_WriteString(const_cast<char*>(str), *font, White);
}

void SSD1306_SIMPLE::println(const char* str, const SSD1306_Font_t* font) {
    // 先用指定字体输出文本。
    print(str, font);
    // 这里暂未实现严格的自动换行，仅保留接口形式，便于后续扩展。
}

int SSD1306_SIMPLE::printf(const SSD1306_Font_t* font, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(font, format, args);
    va_end(args);
    return result;
}

// ####### 基础图形绘制实现 #######

void SSD1306_SIMPLE::drawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    ssd1306_DrawPixel(x, y, color);
}

void SSD1306_SIMPLE::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    ssd1306_Line(x1, y1, x2, y2, color);
}

void SSD1306_SIMPLE::drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    ssd1306_DrawRectangle(x1, y1, x2, y2, color);
}

void SSD1306_SIMPLE::fillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    ssd1306_FillRectangle(x1, y1, x2, y2, color);
}

void SSD1306_SIMPLE::drawCircle(uint8_t x, uint8_t y, uint8_t radius, SSD1306_COLOR color) {
    ssd1306_DrawCircle(x, y, radius, color);
}

void SSD1306_SIMPLE::fillCircle(uint8_t x, uint8_t y, uint8_t radius, SSD1306_COLOR color) {
    ssd1306_FillCircle(x, y, radius, color);
}

// ####### 进度条实现 #######

void SSD1306_SIMPLE::drawProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage,
                                     SSD1306_COLOR borderColor, SSD1306_COLOR fillColor) {
    // 将进度限制在 0~100，避免填充宽度计算越界。
    if (percentage > 100) {
        percentage = 100;
    }

    // 先绘制边框。
    drawRectangle(x, y, x + width - 1, y + height - 1, borderColor);

    // 根据百分比换算内部填充宽度。
    uint8_t fillWidth = (width - 2) * percentage / 100;

    // 边框内部预留 1 像素边距，再绘制填充区域。
    if (fillWidth > 0) {
        fillRectangle(x + 1, y + 1, x + 1 + fillWidth, y + height - 2, fillColor);
    }
}

void SSD1306_SIMPLE::drawProgressBarWithText(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage,
                                             const char* label, const SSD1306_Font_t* font,
                                             SSD1306_COLOR borderColor, SSD1306_COLOR fillColor,
                                             SSD1306_COLOR textColor) {
    (void)textColor;

    // 如果没有指定字体，则使用当前默认字体。
    const SSD1306_Font_t* textFont = (font != nullptr) ? font : currentFont;

    // 先绘制进度条图形部分。
    drawProgressBar(x, y, width, height, percentage, borderColor, fillColor);

    // 计算文字显示位置，这里采用较简单的左对齐 + 垂直居中方案。
    uint8_t textX = x + 2;
    uint8_t textY = y + (height - textFont->height) / 2;

    // 暂存外部原有光标位置，避免函数影响调用方后续排版。
    uint8_t savedCursorX = getCursorX();
    uint8_t savedCursorY = getCursorY();

    // 切换到进度条文字显示位置。
    setCursor(textX, textY);

    // 组织进度条文字内容。
    if (label != nullptr) {
        formatBuffer[0] = '\0';
        const size_t label_length = strlen(label);
        size_t copy_length = (label_length < sizeof(formatBuffer) - 1U) ? label_length : (sizeof(formatBuffer) - 1U);
        memcpy(formatBuffer, label, copy_length);
        size_t index = copy_length;

        if (index + 1U < sizeof(formatBuffer)) {
            formatBuffer[index++] = ' ';
        }

        index = AppendUnsignedLong(formatBuffer, sizeof(formatBuffer), index, percentage);
        index = AppendChar(formatBuffer, sizeof(formatBuffer), index, '%');
        const size_t terminator_index = (index < sizeof(formatBuffer)) ? index : (sizeof(formatBuffer) - 1U);
        formatBuffer[terminator_index] = '\0';
    } else {
        size_t index = 0;
        index = AppendUnsignedLong(formatBuffer, sizeof(formatBuffer), index, percentage);
        index = AppendChar(formatBuffer, sizeof(formatBuffer), index, '%');
        const size_t terminator_index = (index < sizeof(formatBuffer)) ? index : (sizeof(formatBuffer) - 1U);
        formatBuffer[terminator_index] = '\0';
    }

    // 把文字写入缓冲区。
    print(formatBuffer, textFont);

    // 恢复调用前的光标位置。
    setCursor(savedCursorX, savedCursorY);
}

// ####### 屏幕状态控制实现 #######

void SSD1306_SIMPLE::setDisplayOn(bool on) {
    ssd1306_SetDisplayOn(on ? 1 : 0);
}

bool SSD1306_SIMPLE::getDisplayOn() const {
    return ssd1306_GetDisplayOn() != 0;
}

void SSD1306_SIMPLE::setContrast(uint8_t contrast) {
    ssd1306_SetContrast(contrast);
}

// ####### 屏幕参数与字体配置实现 #######

uint8_t SSD1306_SIMPLE::getWidth() const {
    return SSD1306_WIDTH;
}

uint8_t SSD1306_SIMPLE::getHeight() const {
    return SSD1306_HEIGHT;
}

void SSD1306_SIMPLE::setDefaultFont(const SSD1306_Font_t* font) {
    if (font != nullptr) {
        currentFont = font;
    }
}

const SSD1306_Font_t* SSD1306_SIMPLE::getDefaultFont() const {
    return currentFont;
}

// ####### 内部格式化输出实现 #######
// 本函数先把格式化内容写入内部缓冲区，再调用 print() 绘制。
int SSD1306_SIMPLE::vprintf(const SSD1306_Font_t* font, const char* format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    const int len = FormatToBuffer(formatBuffer, sizeof(formatBuffer), format, args_copy);
    va_end(args_copy);

    if (len > 0) {
        // 格式化成功后，统一走 print() 输出到 OLED 缓冲区。
        print(formatBuffer, font);
    }

    return len;
}
