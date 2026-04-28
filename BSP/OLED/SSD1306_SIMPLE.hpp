// ssd1306_simple.hpp
// Created by 22320 on 2025/12/18.
// Modified to add SSD1306_SIMPLE class with printf-like interface

#ifndef FREERTOSDEMO_SSD1306_SIMPLE_H
#define FREERTOSDEMO_SSD1306_SIMPLE_H

#include <cstdint>
#include <cstdarg>
#include "ssd1306.h"
#include "ssd1306_fonts.h"

// ####### SSD1306_SIMPLE 类说明 #######
// 这是对底层 ssd1306 C 驱动的 C++ 二次封装。
// 目标是提供更直观的接口，便于在业务代码中直接完成：
// 1. 初始化屏幕
// 2. 文本输出
// 3. 基础图形绘制
// 4. 进度条显示
// 5. 屏幕开关与对比度控制
//
// 使用流程通常如下：
// 1. 创建 SSD1306_SIMPLE 对象
// 2. 调用 init() 初始化 OLED
// 3. 调用 clear()/setCursor()/print()/drawXXX() 修改缓冲区
// 4. 调用 update() 把缓冲区真正刷新到屏幕

class SSD1306_SIMPLE {
public:
    // ####### 构造与初始化接口 #######

    // 构造函数：创建一个 OLED 对象，并准备默认字体与内部格式化缓冲区。
    SSD1306_SIMPLE();

    // 初始化屏幕：执行底层 SSD1306 初始化流程。
    // 一般在系统外设初始化完成、I2C/SPI 可用之后调用一次即可。
    void init();

    // 清空屏幕缓冲区，并将光标复位到 (0, 0)。
    // 注意：这里只改内存缓冲区，不会立刻显示到屏幕，仍需调用 update()。
    void clear();

    // 刷新屏幕：把当前缓冲区内容发送到 OLED。
    // 所有 print()/drawXXX() 的结果，最终都要靠这个函数显示出来。
    void update();

    // 设置文本光标位置。
    // 后续 print()/printf() 输出文字时，会从该坐标开始绘制。
    void setCursor(uint8_t x, uint8_t y);

    // 读取当前光标 X 坐标。
    // 当前实现中返回的是占位值，后续如果需要精确管理光标，可以继续完善。
    uint8_t getCursorX() const;

    // 读取当前光标 Y 坐标。
    // 当前实现中返回的是占位值，后续如果需要精确管理光标，可以继续完善。
    uint8_t getCursorY() const;

    // ####### 文本输出接口 #######

    // 使用默认字体输出字符串。
    // 输出到内部显存缓冲区，不会自动刷新到屏幕。
    void print(const char* str);

    // 输出字符串并换行。
    // 当前换行逻辑较简化，主要用于接口保留和后续扩展。
    void println(const char* str);

    // 类似 printf 的格式化输出，使用默认字体。
    // 当前支持常用格式：%s %c %d %i %u %ld %li %lu %%
    int printf(const char* format, ...);

    // 使用指定字体输出字符串。
    void print(const char* str, const SSD1306_Font_t* font);

    // 使用指定字体输出字符串并换行。
    void println(const char* str, const SSD1306_Font_t* font);

    // 使用指定字体进行格式化输出。
    int printf(const SSD1306_Font_t* font, const char* format, ...);

    // ####### 基础图形绘制接口 #######

    // 绘制单个像素点。
    void drawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color = White);

    // 绘制直线。
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color = White);

    // 绘制空心矩形。
    void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color = White);

    // 绘制实心矩形。
    void fillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color = White);

    // 绘制空心圆。
    void drawCircle(uint8_t x, uint8_t y, uint8_t radius, SSD1306_COLOR color = White);

    // 绘制实心圆。
    void fillCircle(uint8_t x, uint8_t y, uint8_t radius, SSD1306_COLOR color = White);

    // ####### 进度条显示接口 #######

    // 绘制一个不带文字的进度条。
    // percentage 取值建议为 0~100，函数内部会自动限幅。
    void drawProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage,
                         SSD1306_COLOR borderColor = White, SSD1306_COLOR fillColor = White);

    // 绘制一个带文字的进度条。
    // label 可传 nullptr，此时只显示百分比数字。
    // font 可传 nullptr，此时使用当前默认字体。
    void drawProgressBarWithText(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage,
                                 const char* label = nullptr, const SSD1306_Font_t* font = nullptr,
                                 SSD1306_COLOR borderColor = White, SSD1306_COLOR fillColor = White,
                                 SSD1306_COLOR textColor = White);

    // ####### 屏幕控制接口 #######

    // 设置屏幕开关。
    // true 为开屏，false 为关屏。
    void setDisplayOn(bool on);

    // 读取当前记录的屏幕开关状态。
    bool getDisplayOn() const;

    // 设置对比度。
    // 一般范围为 0~255，数值越大通常越亮。
    void setContrast(uint8_t contrast);

    // ####### 屏幕参数接口 #######

    // 获取屏幕宽度，单位像素。
    uint8_t getWidth() const;

    // 获取屏幕高度，单位像素。
    uint8_t getHeight() const;

    // ####### 字体管理接口 #######

    // 设置默认字体。
    // 后续未显式指定字体时，print()/printf() 会使用这里设置的字体。
    void setDefaultFont(const SSD1306_Font_t* font);

    // 获取当前默认字体。
    const SSD1306_Font_t* getDefaultFont() const;

private:
    // ####### 内部成员 #######

    // 内部格式化缓冲区。
    // 用于 printf()、带文字进度条等文本拼接场景。
    char formatBuffer[128];

    // 当前默认字体指针。
    const SSD1306_Font_t* currentFont;

    // 内部格式化输出实现。
    // 对外由 printf() 调用，对内负责把格式化结果整理到 formatBuffer 中。
    int vprintf(const SSD1306_Font_t* font, const char* format, va_list args);
};

#endif // FREERTOSDEMO_SSD1306_SIMPLE_H
