#pragma once
#include <U8g2lib.h>

class Screen
{
    public:
    Screen(U8G2 &u8g2, uint16_t w = 128, uint16_t h = 64);
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
    void drawAscii(const uint8_t *data, uint16_t w, uint16_t h, uint16_t x, uint16_t y);
    void setColumnOffset(uint8_t offset);
    uint16_t width() const { return _width; }
    uint16_t height() const { return _height; }
    U8G2 &u8g2() { return _u8g2; }
    private:
    U8G2 &_u8g2;
    uint16_t _width;
    uint16_t _height;
};
