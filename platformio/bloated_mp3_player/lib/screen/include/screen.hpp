#pragma once
#include <U8g2lib.h>
#include <SPI.h>

class Screen
{
public:
    Screen(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t sclk, uint8_t mosi, uint8_t miso = 0);
    void begin();
    void clear();
    void display();
    void setFont(const uint8_t *font);
    void setContrast(uint8_t value);
    void setCursor(uint16_t x, uint16_t y);
    void print(const char *text);
    void printAt(const char *text, uint16_t x, uint16_t y);
    void printAt(uint16_t x, uint16_t y, const char *fmt, ...);
    void drawPixel(uint16_t x, uint16_t y);
    void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t r);
    void drawHLine(uint16_t x, uint16_t y, uint16_t w);
    void drawVLine(uint16_t x, uint16_t y, uint16_t h);
    void setColumnOffset(uint8_t offset);
    uint16_t width() const { return 128; }
    uint16_t height() const { return 64; }
private:
    U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI _u8g2;
    uint8_t _cs, _dc, _rst, _sclk, _mosi, _miso;
};
