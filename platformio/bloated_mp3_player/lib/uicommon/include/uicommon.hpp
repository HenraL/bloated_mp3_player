#pragma once
#include <Arduino.h>
#include <leds.hpp>
#include <fonts.hpp>

/**
 * @file uicommon.hpp
 * @brief Abstract Canvas interface and drawing primitives.
 *
 * Provides a hardware-agnostic drawing API that both LcdCanvas and
 * MatrixCanvas implement. Primitives (line, rect, circle, etc.) call
 * pixel() internally, so each backend only needs to implement that +
 * show() + clear().
 */

 /**
  * @brief  2D point.
  */
struct Point { int16_t x, y; };

/**
 * @brief  Abstract drawing surface.
 *
 * Subclasses implement pixel(), clear(), show(), width(), height() and
 * optionally override set_font(), text(), draw_image(), draw_xbm().
 */
class Canvas
{
    public:
    virtual ~Canvas() = default;

    /**
     * @brief Select a baked bitmap font for subsequent text() calls.
     *
     * The pointer must remain valid for the lifetime of the canvas.
     * Pass nullptr to revert to the default (doto_8pt).
     */
    void set_baked_font(const BakedFonts::FontHandle *font);

    protected:
    const BakedFonts::FontHandle *_baked_font = nullptr;

    public:
    // ── Metrics ────────────────────────────────────────────────────────
    /** @return Width of the canvas in pixels. */
    virtual uint16_t width() const = 0;

    /** @return Height of the canvas in pixels. */
    virtual uint16_t height() const = 0;

    // ── Core drawing ────────────────────────────────────────────────────

    /**
     * @brief Set a single pixel.
     * @param x  Column (0 = left).
     * @param y  Row (0 = top).
     * @param c  Colour (My::LED::Colour with r,g,b,w channels).
     */
    virtual void pixel(int16_t x, int16_t y, My::LED::Colour c) = 0;

    /** @brief Fill the entire canvas with colour @p c. */
    virtual void clear(My::LED::Colour c = My::LED::Colour(0, 0, 0, 0)) = 0;

    /** @brief Flush the drawing buffer to the physical display. */
    virtual void show() = 0;

    // ── Shape primitives (call pixel()) ────────────────────────────────

    /** @brief Draw a horizontal line from (x,y) for @p w pixels. */
    void hline(int16_t x, int16_t y, uint16_t w, My::LED::Colour c);

    /** @brief Draw a vertical line from (x,y) for @p h pixels. */
    void vline(int16_t x, int16_t y, uint16_t h, My::LED::Colour c);

    /** @brief Draw a line between two points (Bresenham). */
    void line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, My::LED::Colour c);

    /** @brief Draw a rectangle outline. */
    void rect(int16_t x, int16_t y, uint16_t w, uint16_t h, My::LED::Colour c);

    /** @brief Draw a filled rectangle. */
    void fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, My::LED::Colour c);

    /** @brief Draw a circle outline (Bresenham). */
    void circle(int16_t x0, int16_t y0, uint16_t r, My::LED::Colour c);

    /** @brief Draw a filled circle. */
    void fill_circle(int16_t x0, int16_t y0, uint16_t r, My::LED::Colour c);

    // ── Font / text ────────────────────────────────────────────────────

    /**
     * @brief Switch to a new font for subsequent text() calls.
     *
     * Default no-op (built-in 5x7 bitmap). LcdCanvas overrides to
     * forward to Screen::setFont() for U8G2 font rendering.
     */
    virtual void set_font(const uint8_t *font);

    /**
     * @brief Render a string at (x,y) using the currently selected font.
     * @param x   Left edge in pixels.
     * @param y   Baseline / top edge (backend-dependent).
     * @param str NUL-terminated string.
     * @param c   Text colour.
     */
    virtual void text(int16_t x, int16_t y, const char *str, My::LED::Colour c);

    /**
     * @brief Convenience: set font, then render text.
     *
     * Equivalent to set_font(font); text(x, y, str, c);
     */
    void text(int16_t x, int16_t y, const uint8_t *font,
        const char *str, My::LED::Colour c);

    // ── Images ─────────────────────────────────────────────────────────

    /**
     * @brief Draw a byte-per-pixel grayscale image.
     *
     * Each byte in @p data is a brightness 0-255.
     * The foreground colour @p fg is used for non-zero pixels.
     *
     * LCD  : Bayer-ordered dithering to monochrome.
     * Matrix: byte → RGB brightness scaled by @p fg.
     */
    virtual void draw_image(int16_t x, int16_t y, const uint8_t *data,
        uint16_t w, uint16_t h,
        My::LED::Colour fg = My::LED::Colour(255, 255, 255, 0));

    /**
     * @brief Draw a 1-bit-per-pixel XBM image.
     *
     * @p bits is in standard XBM format (MSB-first, row-padded to byte).
     * LCD uses U8G2 drawXBMP(). Matrix renders set bits as @p fg.
     */
    virtual void draw_xbm(int16_t x, int16_t y, uint16_t w, uint16_t h,
        const uint8_t *bits,
        My::LED::Colour fg = My::LED::Colour(255, 255, 255, 0));
};

/**
 * @brief  Typewriter effect — print one character at a time.
 *
 * Renders @p str at (x,y) on @p cvs, calling show() and delay() after
 * each character so the text appears to be typed out.
 *
 * @param cvs      Target canvas.
 * @param x, y     Starting position.
 * @param str      Text to print.
 * @param col      Colour.
 * @param delay_ms Delay between characters (milliseconds).
 */
void typewrite(Canvas &cvs, int16_t x, int16_t y, const char *str,
    My::LED::Colour col, uint16_t delay_ms = 50);
