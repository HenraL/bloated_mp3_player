#include "screen.hpp"

Screen::Screen(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t sclk, uint8_t mosi, uint8_t miso)
    : _u8g2(U8G2_R0, cs, dc, rst), _cs(cs), _dc(dc), _rst(rst), _sclk(sclk), _mosi(mosi), _miso(miso)
{
}

void Screen::begin()
{
    SPI.begin(_sclk, _miso, _mosi);

    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(10);

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

void Screen::setColumnOffset(uint8_t offset)
{
    digitalWrite(_dc, LOW);
    digitalWrite(_cs, LOW);
    SPI.transfer(0x10 | (offset >> 4));
    SPI.transfer(0x00 | (offset & 0x0F));
    digitalWrite(_cs, HIGH);
    digitalWrite(_dc, HIGH);
}
