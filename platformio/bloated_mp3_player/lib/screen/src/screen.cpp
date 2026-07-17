#include "screen.hpp"

Screen::Screen(U8G2 &u8g2, uint16_t w, uint16_t h)
    : _u8g2(u8g2), _width(w), _height(h)
{
}

void Screen::begin()
{
    _u8g2.begin();
    _u8g2.setContrast(40);
}

void Screen::clear()
{
    _u8g2.clearBuffer();
}

void Screen::display()
{
    _u8g2.sendBuffer();
}

void Screen::setFont(const uint8_t *font)
{
    _u8g2.setFont(font);
}

void Screen::setContrast(uint8_t value)
{
    _u8g2.setContrast(value);
}

void Screen::setCursor(uint16_t x, uint16_t y)
{
    _u8g2.setCursor(x, y);
}

void Screen::print(const char *text)
{
    _u8g2.print(text);
}

void Screen::printAt(const char *text, uint16_t x, uint16_t y)
{
    _u8g2.setCursor(x, y);
    _u8g2.print(text);
}

void Screen::printAt(uint16_t x, uint16_t y, const char *fmt, ...)
{
    char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    _u8g2.setCursor(x, y);
    _u8g2.print(buf);
}

void Screen::drawPixel(uint16_t x, uint16_t y)
{
    _u8g2.drawPixel(x, y);
}

void Screen::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    _u8g2.drawLine(x1, y1, x2, y2);
}

void Screen::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    _u8g2.drawFrame(x, y, w, h);
}

void Screen::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    _u8g2.drawBox(x, y, w, h);
}

void Screen::drawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
    _u8g2.drawCircle(x0, y0, r);
}

void Screen::drawHLine(uint16_t x, uint16_t y, uint16_t w)
{
    _u8g2.drawHLine(x, y, w);
}

void Screen::drawVLine(uint16_t x, uint16_t y, uint16_t h)
{
    _u8g2.drawVLine(x, y, h);
}

static const uint8_t BAYER_4x4[16] = {
     0,  8,  2, 10,
    12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5
};

void Screen::drawAscii(const uint8_t *data, uint16_t w, uint16_t h, uint16_t x, uint16_t y)
{
    for (uint16_t iy = 0; iy < h && y + iy < _height; iy++)
    {
        for (uint16_t ix = 0; ix < w && x + ix < _width; ix++)
        {
            uint8_t v = data[iy * w + ix];
            uint8_t t = BAYER_4x4[(iy & 3) * 4 + (ix & 3)];
            if (v > t * 16)
                _u8g2.drawPixel(x + ix, y + iy);
        }
    }
}
