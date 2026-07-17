#pragma once
#include <uicommon.hpp>
#include <screen.hpp>

/**
 * @file uilcd.hpp
 * @brief LCD canvas — renders Canvas primitives on a U8G2-backed Screen.
 *
 * All drawing ops go to the U8G2 buffer. Call show() to flush.
 * text() uses U8G2 fonts (set via set_font()). draw_image() uses
 * Bayer-ordered dithering for grayscale → monochrome conversion.
 * draw_xbm() delegates to U8G2's native drawXBMP().
 */

/**
 * @brief  Canvas implementation for a monochrome LCD (U8G2).
 *
 * Colour data is accepted but all non-zero colours render as "on".
 * Font selection is forwarded to Screen::setFont().
 */
class LcdCanvas : public Canvas
{
public:
    /** @param screen Reference to an initialized Screen instance. */
    LcdCanvas(Screen &screen);

    uint16_t width() const override;
    uint16_t height() const override;
    void pixel(int16_t x, int16_t y, MY_LED::Colour c) override;
    void clear(MY_LED::Colour c = MY_LED::Colour(0,0,0,0)) override;
    void show() override;

    void set_font(const uint8_t *font) override;
    void text(int16_t x, int16_t y, const char *str,
              MY_LED::Colour c) override;

    void draw_image(int16_t x, int16_t y, const uint8_t *data,
                    uint16_t w, uint16_t h,
                    MY_LED::Colour fg = MY_LED::Colour(255,255,255,0)) override;
    void draw_xbm(int16_t x, int16_t y, uint16_t w, uint16_t h,
                  const uint8_t *bits,
                  MY_LED::Colour fg = MY_LED::Colour(255,255,255,0)) override;
private:
    Screen &_screen;
};
