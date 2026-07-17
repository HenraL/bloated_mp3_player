#pragma once
#include <uicommon.hpp>
#include <matrix.hpp>

/**
 * @file uimatrix.hpp
 * @brief Matrix canvas — renders Canvas primitives on a NeoPixel strip.
 *
 * Maps 2D (x,y) coordinates to LED indices using Matrix::width() as
 * row stride. RGB colours translate directly to NeoPixel values.
 * Images are rendered with smooth brightness scaling (no dithering).
 */

/**
 * @brief  Canvas implementation for an LED matrix (NeoPixel strip).
 *
 * Coordinates are clipped to [0, width()) × [0, height()).
 * The built-in 5x7 font is used for text (U8G2 fonts are ignored on
 * this backend).
 */
class MatrixCanvas : public Canvas
{
public:
    MatrixCanvas();

    uint16_t width() const override;
    uint16_t height() const override;
    void pixel(int16_t x, int16_t y, MY_LED::Colour c) override;
    void clear(MY_LED::Colour c = MY_LED::Colour(0,0,0,0)) override;
    void show() override;

    void draw_image(int16_t x, int16_t y, const uint8_t *data,
                    uint16_t w, uint16_t h,
                    MY_LED::Colour fg = MY_LED::Colour(255,255,255,0)) override;
private:
    /** @brief Convert (x,y) to NeoPixel strip index. */
    uint16_t _idx(int16_t x, int16_t y) const;
};
