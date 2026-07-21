#include "uilcd.hpp"

LcdCanvas::LcdCanvas(Screen &screen) : _screen(screen) {}

uint16_t LcdCanvas::width() const { return _screen.width(); }
uint16_t LcdCanvas::height() const { return _screen.height(); }

void LcdCanvas::pixel(int16_t x, int16_t y, My::LED::Colour c)
{
    (void)c;
    _screen.drawPixel(x, y);
}

void LcdCanvas::clear(My::LED::Colour c)
{
    (void)c;
    _screen.clear();
}

void LcdCanvas::show()
{
    _screen.display();
}

void LcdCanvas::set_font(const uint8_t *font)
{
    _screen.setFont(font);
}

void LcdCanvas::text(int16_t x, int16_t y, const char *str, My::LED::Colour c)
{
    (void)c;
    _screen.printAt(x, y, str);
}

void LcdCanvas::draw_image(int16_t x, int16_t y, const uint8_t *data,
    uint16_t w, uint16_t h, My::LED::Colour fg)
{
    (void)fg;
    _screen.drawAscii(data, w, h, x, y);
}

void LcdCanvas::draw_xbm(int16_t x, int16_t y, uint16_t w, uint16_t h,
    const uint8_t *bits, My::LED::Colour fg)
{
    (void)fg;
    _screen.u8g2().drawXBMP(x, y, w, h, bits);
}
