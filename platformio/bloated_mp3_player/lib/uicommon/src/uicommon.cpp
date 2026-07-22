#include "uicommon.hpp"

/**
 * @file uicommon.cpp
 * @brief Default implementations of Canvas drawing primitives.
 *
 * All shape primitives (line, rect, circle, etc.) are implemented in
 * terms of pixel() so backends only need to supply that one function.
 */

 // ── Primitives ─────────────────────────────────────────────────────────

void Canvas::hline(int16_t x, int16_t y, uint16_t w, My::LED::Colour c)
{
    for (uint16_t i = 0; i < w; i++)
        pixel(x + i, y, c);
}

void Canvas::vline(int16_t x, int16_t y, uint16_t h, My::LED::Colour c)
{
    for (uint16_t i = 0; i < h; i++)
        pixel(x, y + i, c);
}

void Canvas::line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, My::LED::Colour c)
{
    int16_t dx = abs(x2 - x1), dy = -abs(y2 - y1);
    int16_t sx, sy;
    if (x1 < x2) { sx = 1; } else { sx = -1; }
    if (y1 < y2) { sy = 1; } else { sy = -1; }
    int16_t err = dx + dy, e2;
    while (true) {
        pixel(x1, y1, c);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void Canvas::rect(int16_t x, int16_t y, uint16_t w, uint16_t h, My::LED::Colour c)
{
    hline(x, y, w, c);
    hline(x, y + h - 1, w, c);
    vline(x, y, h, c);
    vline(x + w - 1, y, h, c);
}

void Canvas::fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, My::LED::Colour c)
{
    for (uint16_t iy = 0; iy < h; iy++)
        hline(x, y + iy, w, c);
}

void Canvas::circle(int16_t x0, int16_t y0, uint16_t r, My::LED::Colour c)
{
    int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
    pixel(x0, y0 + r, c); pixel(x0, y0 - r, c);
    pixel(x0 + r, y0, c); pixel(x0 - r, y0, c);
    while (x < y) {
        if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
        x++; ddF_x += 2; f += ddF_x;
        pixel(x0 + x, y0 + y, c); pixel(x0 - x, y0 + y, c);
        pixel(x0 + x, y0 - y, c); pixel(x0 - x, y0 - y, c);
        pixel(x0 + y, y0 + x, c); pixel(x0 - y, y0 + x, c);
        pixel(x0 + y, y0 - x, c); pixel(x0 - y, y0 - x, c);
    }
}

void Canvas::fill_circle(int16_t x0, int16_t y0, uint16_t r, My::LED::Colour c)
{
    int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
    for (int16_t i = y0 - r; i <= y0 + r; i++) pixel(x0, i, c);
    while (x < y) {
        if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
        x++; ddF_x += 2; f += ddF_x;
        for (int16_t i = y0 - y; i <= y0 + y; i++) { pixel(x0 + x, i, c); pixel(x0 - x, i, c); }
        for (int16_t i = y0 - x; i <= y0 + x; i++) { pixel(x0 + y, i, c); pixel(x0 - y, i, c); }
    }
}

// ── Font / text ────────────────────────────────────────────────────────

// Default fallback: baked doto 8pt
static const BakedFonts::FontHandle _default_font = {
    BakedFonts::doto_8pt_codes,
    BakedFonts::doto_8pt_bits,
    BakedFonts::doto_8pt_widths,
    BakedFonts::DOTO_8PT_COUNT,
    BakedFonts::DOTO_8PT_WIDTH,
    BakedFonts::DOTO_8PT_HEIGHT,
};

void Canvas::set_baked_font(const BakedFonts::FontHandle *font)
{
    _baked_font = font;
}

void Canvas::set_font(const uint8_t *font)
{
    (void)font;
}

static uint32_t utf8_decode(const char *&s)
{
    uint8_t c = (uint8_t)*s;
    if (c < 0x80) {
        s++;
        return c;
    }
    if (c < 0xC0) {
        s++;
        return 0xFFFD;
    }
    uint32_t cp;
    uint8_t extra;
    if (c < 0xE0) { cp = c & 0x1F; extra = 1; }
    else if (c < 0xF0) { cp = c & 0x0F; extra = 2; }
    else if (c < 0xF8) { cp = c & 0x07; extra = 3; }
    else { s++; return 0xFFFD; }
    for (uint8_t i = 0; i < extra; i++) {
        s++;
        uint8_t b = (uint8_t)*s;
        if ((b & 0xC0) != 0x80) return 0xFFFD;
        cp = (cp << 6) | (b & 0x3F);
    }
    s++;
    return cp;
}

static uint16_t find_glyph(
    const uint32_t *codes, uint16_t count, uint32_t cp)
{
    int16_t lo = 0;
    int16_t hi = (int16_t)(count - 1);
    while (lo <= hi) {
        uint16_t mid = (uint16_t)((lo + hi) / 2);
        if (codes[mid] == cp) return mid;
        if (codes[mid] < cp) lo = (int16_t)(mid + 1);
        else hi = (int16_t)(mid - 1);
    }
    return 0xFFFF;
}

void Canvas::text(int16_t x, int16_t y, const char *str, My::LED::Colour c)
{
    const BakedFonts::FontHandle *fh =
        _baked_font ? _baked_font : &_default_font;
    uint16_t count = fh->count;
    uint8_t cell_h = fh->glyph_height;

    while (*str) {
        uint32_t cp = utf8_decode(str);
        if (cp == 0xFFFD) continue;

        if (cp == 0x20) {
            uint16_t idx = find_glyph(fh->codes, count, cp);
            if (idx != 0xFFFF && fh->widths[idx] > 0) {
                x += fh->widths[idx];
            }
            else {
                x += fh->glyph_width / 2 + 1;
            }
            continue;
        }

        uint16_t idx = find_glyph(fh->codes, count, cp);
        if (idx != 0xFFFF) {
            uint8_t gw = fh->widths[idx];
            if (gw == 0) continue;

            uint8_t row_bytes = (gw + 7) / 8;

            uint32_t bit_off = 0;
            for (uint16_t i = 0; i < idx; i++)
                if (fh->widths[i] != 0)
                    bit_off += ((fh->widths[i] + 7) / 8) * cell_h;

            for (uint8_t row = 0; row < cell_h; row++) {
                uint32_t ro = bit_off + row * row_bytes;
                for (uint8_t col = 0; col < gw; col++) {
                    uint8_t b = pgm_read_byte(&fh->bits[ro + col / 8]);
                    if (b & (0x80 >> (col & 7)))
                        pixel(x + col, y + row, c);
                }
            }
            x += gw + 1;
        }
    }
}

void Canvas::text(int16_t x, int16_t y, const uint8_t *font,
    const char *str, My::LED::Colour c)
{
    set_font(font);
    text(x, y, str, c);
}

// ── Images ─────────────────────────────────────────────────────────────

static const uint8_t BAYER_4x4[16] = {
     0,  8,  2, 10,
    12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5
};

void Canvas::draw_image(int16_t x, int16_t y, const uint8_t *data,
    uint16_t w, uint16_t h, My::LED::Colour fg)
{
    for (uint16_t iy = 0; iy < h; iy++) {
        for (uint16_t ix = 0; ix < w; ix++) {
            uint8_t v = data[iy * w + ix];
            uint8_t t = BAYER_4x4[(iy & 3) * 4 + (ix & 3)];
            if (v > t * 16)
                pixel(x + ix, y + iy, fg);
        }
    }
}

void Canvas::draw_xbm(int16_t x, int16_t y, uint16_t w, uint16_t h,
    const uint8_t *bits, My::LED::Colour fg)
{
    for (uint16_t iy = 0; iy < h; iy++) {
        for (uint16_t ix = 0; ix < w; ix++) {
            uint16_t byte_idx = iy * ((w + 7) / 8) + ix / 8;
            uint8_t bit = bits[byte_idx] & (1 << (7 - (ix % 8)));
            if (bit)
                pixel(x + ix, y + iy, fg);
        }
    }
}

// ── Typewrite ──────────────────────────────────────────────────────────

void typewrite(Canvas &cvs, int16_t x, int16_t y, const char *str,
    My::LED::Colour col, uint16_t delay_ms)
{
    while (*str) {
        char buf[2] = { *str, '\0' };
        cvs.text(x, y, buf, col);
        cvs.show();
        delay(delay_ms);
        x += 6; // fixed advance for typewriter effect
        str++;
    }
}
