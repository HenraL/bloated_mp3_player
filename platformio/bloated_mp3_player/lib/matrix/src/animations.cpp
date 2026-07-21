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
* FILE: animations.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 22:54:44 20-07-2026
* DESCRIPTION:
* Implementation of the RGB matrix animations. Each frame is computed
* with the kind of brute-force enthusiasm that would make a Vogon proud.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Makes the LEDs do pretty (or terrifying) things.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/animations.hpp"
#include "internal/config.hpp"
#include <internal/colours.hpp>

// 8-bit sine approximation (FastLED-compatible)
static const uint8_t sin8_lut[256] PROGMEM = {
    128,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173,
    176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,
    218,220,222,224,226,228,229,231,233,234,236,237,238,240,241,242,
    243,244,245,245,246,247,247,248,248,249,249,249,249,249,249,249,
    249,249,249,249,249,249,249,248,248,247,247,246,245,245,244,243,
    242,241,240,238,237,236,234,233,231,229,228,226,224,222,220,218,
    215,213,211,208,206,203,201,198,196,193,190,188,185,182,179,176,
    173,170,167,165,162,158,155,152,149,146,143,140,137,134,131,128,
    124,121,118,115,112,109,106,103,100,97,93,90,88,85,82,79,
    76,73,70,67,65,62,59,57,54,52,49,47,44,42,40,37,
    35,33,31,29,27,26,24,22,21,19,18,16,15,14,12,11,
    10,9,8,8,7,6,6,5,5,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,5,5,6,6,7,8,8,9,10,
    11,12,14,15,16,18,19,21,22,24,26,27,29,31,33,35,
    37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76,
    79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124
};

static inline uint8_t sin8(uint8_t input)
{
    return pgm_read_byte(&sin8_lut[input]);
}

static Matrix::AnimationState state;
static uint16_t s_led_count = 64;

void Matrix::begin(uint16_t count, uint16_t row_width)
{
    if (count > 0 && count <= MATRIX_MAX_LEDS) { s_led_count = count; } else { s_led_count = MATRIX_MAX_LEDS; }
    if (row_width > 0) { state.row_width = row_width; } else { state.row_width = 8; }
    My::LED::LedStrip.setBrightness(MATRIX_BRIGHTNESS);
    state.last_tick = millis();
}

uint16_t Matrix::led_count() { return s_led_count; }
uint16_t Matrix::width() { return state.row_width; }
uint16_t Matrix::height() { return s_led_count / state.row_width; }

void Matrix::set_animation(Animation anim)
{
    state.current = anim;
    state.frame = 0;
    state.hue_offset = 0;
}

Matrix::Animation Matrix::get_animation()
{
    return state.current;
}

void Matrix::tick()
{
    if (!state.active) return;
    uint32_t now = millis();
    if (now - state.last_tick < state.tick_interval) return;
    state.last_tick = now;
    state.frame++;
    state.hue_offset++;

    switch (state.current) {
        case Animation::Snake:      tick_snake();      break;
        case Animation::Circle:     tick_circle();     break;
        case Animation::Pulse:      tick_pulse();      break;
        case Animation::Rainbow:    tick_rainbow();    break;
        case Animation::BabelFish:  tick_babel_fish(); break;
        case Animation::SoundBars:
        case Animation::Off:
        default:                                         break;
    }
    My::LED::led_refresh();
}

void Matrix::tick_snake()
{
    for (uint16_t i = 0; i < s_led_count; i++) {
        int16_t pos = (i + state.frame * MATRIX_SNAKE_SPEED) % s_led_count;
        if (pos < 4) {
            uint8_t bright;
            if (pos == 0) { bright = 255; } else { bright = (4 - pos) * 64; }
            uint32_t col = My::LED::LedStrip.Color(0, bright, 0, 0);
            My::LED::LedStrip.setPixelColor(i, col);
        } else {
            My::LED::LedStrip.setPixelColor(i, 0);
        }
    }
}

void Matrix::tick_circle()
{
    uint8_t wave = (sin8(state.frame * MATRIX_CIRCLE_SPEED) >> 4) + 1;
    for (uint16_t i = 0; i < s_led_count; i++) {
        uint8_t dist = abs((int)(i - s_led_count / 2));
        if (dist < wave) {
            My::LED::LedStrip.setPixelColor(i,
                My::LED::LedStrip.Color(0, 64, 128, 0));
        } else {
            My::LED::LedStrip.setPixelColor(i, 0);
        }
    }
}

void Matrix::tick_pulse()
{
    uint8_t brightness = sin8(state.frame * MATRIX_PULSE_SPEED);
    uint32_t col = My::LED::LedStrip.Color(
        brightness, brightness / 2, brightness / 4, 0);
    My::LED::LedStrip.fill(col);
}

void Matrix::tick_rainbow()
{
    for (uint16_t i = 0; i < s_led_count; i++) {
        uint8_t hue = (i * 256 / s_led_count) + state.frame * MATRIX_RAINBOW_SPEED;
        uint32_t col = My::LED::LedStrip.ColorHSV(hue * 256, 255, 128);
        My::LED::LedStrip.setPixelColor(i, col);
    }
}

void Matrix::tick_sound_bars(uint8_t *audio_data, size_t len)
{
    if (!audio_data || len == 0) {
        for (uint16_t i = 0; i < s_led_count; i++)
            My::LED::LedStrip.setPixelColor(i, 0);
        return;
    }
    uint16_t bins;
    if (s_led_count < len) { bins = s_led_count; } else { bins = len; }
    for (uint16_t i = 0; i < bins; i++) {
        uint8_t level = audio_data[i * len / bins] >> 4;
        if (level > 15) level = 15;
        uint32_t col = My::LED::LedStrip.ColorHSV(
            level * 256 / 16, 255, level * 16);
        My::LED::LedStrip.setPixelColor(i, col);
    }
    for (uint16_t i = bins; i < s_led_count; i++)
        My::LED::LedStrip.setPixelColor(i, 0);
}

void Matrix::tick_babel_fish()
{
    uint16_t fish_len = 6;
    int16_t fish_pos = (state.frame * MATRIX_BABELFISH_SPEED) % (s_led_count + fish_len) - fish_len;
    for (uint16_t i = 0; i < s_led_count; i++) {
        int16_t offset = (int16_t)i - fish_pos;
        if (offset >= 0 && offset < fish_len) {
            uint8_t phase;
            if (offset == 0 || offset == fish_len - 1) { phase = 80; } else { phase = 255; }
            My::LED::LedStrip.setPixelColor(i,
                My::LED::LedStrip.Color(0, phase, phase / 2, 0));
        } else {
            uint8_t dim;
            if (i % 3 == 0) { dim = 8; } else { dim = 0; }
            My::LED::LedStrip.setPixelColor(i,
                My::LED::LedStrip.Color(0, 0, dim, 0));
        }
    }
}

void Matrix::drawAscii(const uint8_t *data, uint16_t w, uint16_t h,
    uint16_t x, uint16_t y)
{
    uint16_t rw = state.row_width;
    for (uint16_t iy = 0; iy < h && (y + iy) * rw + x < s_led_count; iy++) {
        for (uint16_t ix = 0; ix < w; ix++) {
            uint16_t idx = (y + iy) * rw + x + ix;
            if (idx >= s_led_count) break;
            uint8_t v = data[iy * w + ix];
            My::LED::Colour c(v, v, v, My::LED::WHITE_LEVEL);
            uint32_t col = My::LED::LedStrip.Color(c.r, c.g, c.b, c.w);
            My::LED::LedStrip.setPixelColor(idx, col);
        }
    }
    My::LED::led_refresh();
}

void Matrix::babel_fish_splash(Screen &display)
{
    display.clear();
    display.setFont(u8g2_font_ncenB08_tr);
    display.printAt("Don't Panic!", 0, 12);
    display.setFont(u8g2_font_tom_thumb_4x6_tr);
    display.printAt(0, 24, "The Babel Fish");
    display.printAt(0, 32, "is loosely in");
    display.printAt(0, 40, "your ear canal.");
    display.printAt(0, 50, "...and in the LEDs");
    display.display();
}
