/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* .......................
* ...><>.............<><.
* ..><>.><>.......<><.<><
* .><>.<><.><>.<><.<><.<>
* ..><>.><>.......<><.<><
* ...><>.............<><.
* .......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: my_lcd.cpp
* CREATION DATE: 22-07-2026
* LAST Modified: 21:12:15 22-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file containing the implementation of the functions located in the hpp file.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include <SPI.h>
#include "my/lcd.hpp"
#include "my/config/pins.hpp"

static const uint8_t BAYER_4x4[16] = {
     0,  8,  2, 10,
    12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5
};

// ── Construction & initialisation ─────────────────────────────────────

My::LCD::Display::Display()
    : _u8g2(U8G2_R0,
        My::Config::Pins::LCD_CS,
        My::Config::Pins::LCD_DC,
        My::Config::Pins::LCD_RST)
{
}

void My::LCD::Display::initialise()
{
    SPI.begin(My::Config::Pins::LCD_SCLK,
        My::Config::Pins::IC_SO,
        My::Config::Pins::LCD_MOSI);
    pinMode(My::Config::Pins::LCD_RST, OUTPUT);
    digitalWrite(My::Config::Pins::LCD_RST, LOW);
    delay(10);
    digitalWrite(My::Config::Pins::LCD_RST, HIGH);
    delay(10);
    _initialised = true;
}

// ── Lifecycle ─────────────────────────────────────────────────────────

void My::LCD::Display::begin()
{
    _u8g2.begin();
    _u8g2.setContrast(40);
}

void My::LCD::Display::clear(My::LED::Colour)
{
    _u8g2.clearBuffer();
}

void My::LCD::Display::show()
{
    _u8g2.sendBuffer();
}

// ── Font ──────────────────────────────────────────────────────────────

void My::LCD::Display::setFont(const uint8_t *font)
{
    _baked_font = nullptr;
    _u8g2.setFont(font);
}

void My::LCD::Display::setFont(const BakedFonts::FontHandle *font)
{
    set_baked_font(font);
}

void My::LCD::Display::setContrast(uint8_t value)
{
    _u8g2.setContrast(value);
}

void My::LCD::Display::text(int16_t x, int16_t y, const char *str, My::LED::Colour c)
{
    if (_baked_font) {
        Canvas::text(x, y, str, c);
    } else {
        printAt(str, x, y);
    }
}

// ── Primitives ────────────────────────────────────────────────────────

void My::LCD::Display::drawPixel(uint16_t x, uint16_t y)
{
    _u8g2.drawPixel(x, y);
}

void My::LCD::Display::pixel(int16_t x, int16_t y, My::LED::Colour)
{
    drawPixel(x, y);
}

void My::LCD::Display::drawLine(uint16_t x1, uint16_t y1,
    uint16_t x2, uint16_t y2)
{
    _u8g2.drawLine(x1, y1, x2, y2);
}

void My::LCD::Display::drawRect(uint16_t x, uint16_t y,
    uint16_t w, uint16_t h)
{
    _u8g2.drawFrame(x, y, w, h);
}

void My::LCD::Display::fillRect(uint16_t x, uint16_t y,
    uint16_t w, uint16_t h)
{
    _u8g2.drawBox(x, y, w, h);
}

void My::LCD::Display::drawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
    _u8g2.drawCircle(x0, y0, r);
}

void My::LCD::Display::drawHLine(uint16_t x, uint16_t y, uint16_t w)
{
    _u8g2.drawHLine(x, y, w);
}

void My::LCD::Display::drawVLine(uint16_t x, uint16_t y, uint16_t h)
{
    _u8g2.drawVLine(x, y, h);
}

// ── Text ──────────────────────────────────────────────────────────────

void My::LCD::Display::setCursor(uint16_t x, uint16_t y)
{
    _u8g2.setCursor(x, y);
}

void My::LCD::Display::print(const char *text)
{
    _u8g2.print(text);
}

void My::LCD::Display::printAt(const char *text, uint16_t x, uint16_t y)
{
    if (_baked_font) {
        Canvas::text(x, y, text, My::LED::Colour(255, 255, 255, 0));
    } else {
        _u8g2.setCursor(x, y);
        _u8g2.print(text);
    }
}

void My::LCD::Display::printAt(uint16_t x, uint16_t y,
    const char *fmt, ...)
{
    char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (_baked_font) {
        Canvas::text(x, y, buf, My::LED::Colour(255, 255, 255, 0));
    } else {
        _u8g2.setCursor(x, y);
        _u8g2.print(buf);
    }
}

// ── Images ────────────────────────────────────────────────────────────

void My::LCD::Display::drawAscii(const uint8_t *data, uint16_t w,
    uint16_t h, uint16_t x, uint16_t y)
{
    for (uint16_t iy = 0; iy < h && y + iy < _height; iy++) {
        for (uint16_t ix = 0; ix < w && x + ix < _width; ix++) {
            uint8_t v = data[iy * w + ix];
            uint8_t t = BAYER_4x4[(iy & 3) * 4 + (ix & 3)];
            if (v > t * 16)
                _u8g2.drawPixel(x + ix, y + iy);
        }
    }
}

void My::LCD::Display::draw_image(int16_t x, int16_t y,
    const uint8_t *data, uint16_t w,
    uint16_t h, My::LED::Colour)
{
    drawAscii(data, w, h, x, y);
}

void My::LCD::Display::draw_xbm(int16_t x, int16_t y,
    uint16_t w, uint16_t h,
    const uint8_t *bits, My::LED::Colour)
{
    _u8g2.drawXBMP(x, y, w, h, bits);
}

// ── Metrics ───────────────────────────────────────────────────────────

uint16_t My::LCD::Display::width() const
{
    return _width;
}

uint16_t My::LCD::Display::height() const
{
    return _height;
}
