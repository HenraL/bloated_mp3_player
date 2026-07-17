#pragma once
#include <uicommon.hpp>
#include <math.h>

/**
 * @file turtle.hpp
 * @brief LOGO-style turtle graphics on any Canvas.
 *
 * Provides a minimal Python-turtle-compatible API: forward, backward,
 * turn, pen up/down, goto, circle, and basic fill. All drawing is
 * forwarded to the underlying Canvas so it works on both LCD and
 * matrix backends.
 */

/**
 * @brief  Turtle cursor that draws on a Canvas.
 *
 * State: position (x,y), heading (degrees), pen state, colour.
 */
class Turtle
{
public:
    /** @param canvas The Canvas to draw on. */
    Turtle(Canvas &canvas);

    /** @brief Move forward @p distance pixels, drawing if pen is down. */
    void forward(int16_t distance);

    /** @brief Move backward @p distance pixels. */
    void backward(int16_t distance);

    /** @brief Turn left by @p angle degrees. */
    void left(int16_t angle);

    /** @brief Turn right by @p angle degrees. */
    void right(int16_t angle);

    /** @brief Set heading to @p angle degrees (0 = right, 90 = up). */
    void setheading(int16_t angle);

    /** @return Current heading in degrees. */
    int16_t heading() const;

    /** @brief Lift the pen (stop drawing on movement). */
    void penup();

    /** @brief Lower the pen (resume drawing on movement). */
    void pendown();

    /** @brief true if the pen is down. */
    bool isdown() const;

    /** @brief Move (and draw) to absolute pixel coordinates (x,y). */
    void goto_point(int16_t x, int16_t y);

    /** @brief Return to centre of canvas, heading 90° (up). */
    void home();

    /** @return Current position. */
    Point pos() const;

    /** @brief Set drawing colour. */
    void set_colour(MY_LED::Colour c);

    /** @brief Set line width in pixels (best-effort; not all backends support >1). */
    void set_width(uint8_t w);

    /**
     * @brief Draw a circle.
     * @param radius Positive = clockwise, negative = counter-clockwise.
     */
    void circle(int16_t radius);

    /** @brief Begin recording a filled shape (closed by end_fill()). */
    void begin_fill();

    /** @brief Close and fill the recorded shape (simple bounding-box fill). */
    void end_fill();

private:
    Canvas &_canvas;
    int16_t _x = 0, _y = 0;
    int16_t _angle = 90;
    bool _pen_down = true;
    MY_LED::Colour _colour = MY_LED::Colour(255,255,255,0);
    uint8_t _width = 1;
    bool _filling = false;
    int16_t _fill_start_x = 0, _fill_start_y = 0;
    int16_t _fill_min_x = 32767, _fill_max_x = -32768;
    int16_t _fill_min_y = 32767, _fill_max_y = -32768;
};
