#include "uimatrix.hpp"

MatrixCanvas::MatrixCanvas() {}

uint16_t MatrixCanvas::width() const { return Matrix::width(); }
uint16_t MatrixCanvas::height() const { return Matrix::height(); }

uint16_t MatrixCanvas::_idx(int16_t x, int16_t y) const
{
    return (uint16_t)y * Matrix::width() + (uint16_t)x;
}

void MatrixCanvas::pixel(int16_t x, int16_t y, MY_LED::Colour c)
{
    if (x < 0 || x >= (int16_t)Matrix::width() ||
        y < 0 || y >= (int16_t)Matrix::height())
        return;
    MY_LED::LedStrip.setPixelColor(_idx(x, y),
        MY_LED::LedStrip.Color(c.r, c.g, c.b, c.w));
}

void MatrixCanvas::clear(MY_LED::Colour c)
{
    for (uint16_t i = 0; i < Matrix::led_count(); i++)
    {
        MY_LED::LedStrip.setPixelColor(i,
            MY_LED::LedStrip.Color(c.r, c.g, c.b, c.w));
    }
}

void MatrixCanvas::show()
{
    MY_LED::led_refresh();
}

void MatrixCanvas::draw_image(int16_t x, int16_t y, const uint8_t *data,
                              uint16_t w, uint16_t h, MY_LED::Colour fg)
{
    float scale_r = fg.r / 255.0f;
    float scale_g = fg.g / 255.0f;
    float scale_b = fg.b / 255.0f;
    for (uint16_t iy = 0; iy < h; iy++)
    {
        for (uint16_t ix = 0; ix < w; ix++)
        {
            uint8_t v = data[iy * w + ix];
            pixel(x + ix, y + iy, MY_LED::Colour(
                (uint8_t)(v * scale_r),
                (uint8_t)(v * scale_g),
                (uint8_t)(v * scale_b), 0));
        }
    }
}
