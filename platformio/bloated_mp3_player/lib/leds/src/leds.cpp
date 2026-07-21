/*
* +==== BEGIN CatFeeder =================+
* LOGO:
* ..............(....⁄\
* ...............)..(.')
* ..............(../..)
* ...............\(__)|
* Inspired by Joan Stark
* source https://www.asciiart.eu/
* animals/cats
* /STOP
* PROJECT: CatFeeder
* FILE: leds.cpp
* CREATION DATE: 07-02-2026
* LAST Modified: 11:14:8 18-02-2026
* DESCRIPTION:
* This is the project in charge of making the connected cat feeder project work.
* /STOP
* COPYRIGHT: (c) Cat Feeder
* PURPOSE: These are the functions for the LED module.
* // AR
* +==== END CatFeeder =================+
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "internal/leds.hpp"
#include "internal/config.hpp"
#include "internal/my_utils.hpp"

const My::LED::Colour My::LED::white_colour = My::LED::get_colour_from_pointer(&My::LED::Colours::White);
const My::LED::Colour My::LED::black_colour = My::LED::get_colour_from_pointer(&My::LED::Colours::Black);
const My::LED::Colour My::LED::red_colour = My::LED::get_colour_from_pointer(&My::LED::Colours::Red);
const My::LED::Colour My::LED::yellow_colour = My::LED::get_colour_from_pointer(&My::LED::Colours::Yellow);
const My::LED::Colour My::LED::green_colour = My::LED::get_colour_from_pointer(&My::LED::Colours::Green);
const My::LED::Colour My::LED::blue_colour = My::LED::get_colour_from_pointer(&My::LED::Colours::Blue);
const My::LED::Colour My::LED::dark_blue = My::LED::get_colour_from_pointer(&My::LED::Colours::DarkCyan);

My::LED::LED::LED(const uint32_t led_number, const uint8_t led_pin, const neoPixelType led_type, const My::LED::Colour default_foreground, const My::LED::Colour default_background, const My::LED::Colour forced_colour, const uint32_t forced_colour_end_time, const uint8_t led_brightness)
    : _led_number(led_number), _led_pin(led_pin), _led_brightness(led_brightness), _led_type(led_type), _led_strip(led_number, _led_pin, led_type), _default_foreground(default_foreground), _default_background(default_background), _forced_colour_value(forced_colour), _forced_colour_end_time(forced_colour_end_time)
{
    _led_strip_initialized = false;
    _leds_enabled = true;
    _forced_colour = false;
}

My::LED::LED::~LED() {}

/**
 * @brief Clear (set) pixels after `count` to a background colour and return the effective count.
 *
 * The function clamps `count` to a valid range and writes `colour` to
 * pixels in the range [count .. LED_NUMBER-1]. Returns the clamped value.
 *
 * @param count Number of LEDs to keep active (defaults to `LED_NUMBER`).
 * @param colour Background `Colour` to write to remaining pixels.
 * @return int16_t Effective (clamped) count.
 */
int16_t My::LED::LED::_clear_remaining_count(int16_t count, const Colour &colour)
{
    if (count < 0 || count > LED_NUMBER) {
        count = LED_NUMBER;
    }
    const uint32_t col = _led_strip.Color(colour.r, colour.g, colour.b, colour.w);
    for (uint16_t i = count; i < LED_NUMBER; i++) {
        _led_strip.setPixelColor(i, col); // turn off remaining LEDs
    }
    return count;
}

/**
 * @brief Write `colour` to the first `count` pixels and fill the remainder with `background`.
 *
 * `count` is clamped to a valid range. Pixels [0..count-1] are set to
 * `colour` (using the strip's `Color()` helper). Remaining pixels are
 * written with `background` and the strip is updated with `LedStrip.show()`.
 *
 * @param colour Foreground `Colour` to display (defaults to `default_foreground`).
 * @param count Number of pixels to set (default: all pixels).
 * @param background Background `Colour` to write to remaining pixels.
 */
void My::LED::LED::_fill_colour(const My::LED::Colour &colour, int16_t count, const My::LED::Colour &background)
{
    count = _clamp_count(count);
    const uint32_t FgCol = _led_strip.Color(colour.r, colour.g, colour.b, colour.w);
    for (uint16_t i = 0; i < count; i++) {
        _led_strip.setPixelColor(i, FgCol);
    }
    _clear_remaining_count(count, background);
    _led_strip.show();
}

void My::LED::LED::_process_timer(const uint32_t duration)
{
    if (duration > 0) {
        _forced_colour_end_time = millis() + duration;
    } else {
        _forced_colour_end_time = 0; // infinite
    }
}

/**
 * @brief Move a node's position according to its step and timing settings.
 *
 * This helper advances `item.pos` by `item.pos_step` when the node's
 * internal tick timer indicates it is time to update. Positions are
 * clamped within `[0 .. LED_NUMBER-1]`. Node wrapping/disabling is triggered
 * only when the computed new position would exceed bounds, avoiding off-by-one
 * issues where nodes wrap prematurely when simply reaching the edge.
 *
 * Fixed in recent updates:
 * - Uses int32_t arithmetic to prevent unsigned underflow
 * - Wrap condition based on computed position, not current position
 * - Proper bounds checking for both directions
 *
 * @param item `ColourPos` node to move.
 * @param pos Current reference position (unused but kept for compatibility).
 */
void My::LED::LED::_move_pixel(ColourPos &item, const uint16_t pos)
{
    item.tick_animation.tick();
    if (!item.tick_animation.ticked()) {
        return; // not time yet
    }

    // CRITICAL: Use int32_t to safely handle uint16_t + int16_t arithmetic
    // This prevents unsigned underflow when pos_step is negative
    int32_t new_pos = (int32_t)item.pos + (int32_t)item.pos_step;

    // Clamp position within bounds
    int16_t new_pos_clamped = _clamp_index_inclusif((int16_t)new_pos);

    item.pos = (uint16_t)new_pos_clamped;

    // Optional: auto-disable after hitting the end.
    // Only trigger wrap/disable when the computed `new_pos` goes beyond bounds.
    // This prevents wrapping when the node simply reaches the last index
    // but hasn't attempted to move past it (avoids off-by-one).
    if ((item.pos_step >= 0 && new_pos > (int32_t)_led_number - 1) ||
        (item.pos_step < 0 && new_pos < 0)) {
        if (item.disable_on_complete) {
            item.node_enabled = false;
        } else {
            if (item.pos_step >= 0) {
                item.pos = 0; // wrap to start
            } else {
                item.pos = _led_number - 1; // wrap to end
            }
        }
    }
}


void My::LED::LED::init()
{
    if (_led_strip_initialized) {
        return;
    }
    _led_strip.begin();             // initialize GPIO / strip
    _led_strip.setBrightness(_led_brightness);
    _led_strip.show();              // clear LEDs
    _led_strip_initialized = true;
}

void My::LED::LED::off()
{
    _leds_enabled = false;
    _led_strip.clear();
    _led_strip.show();
}

void My::LED::LED::on()
{
    _leds_enabled = true;
    _forced_colour = false;
    _led_strip.begin();
    _led_strip.setBrightness(_led_brightness);
    _led_strip.show();
}

void My::LED::LED::clear()
{
    _forced_colour = false;
    _led_strip.clear();
    _led_strip.show();
}

void My::LED::LED::step(int16_t count)
{
    _update_forced_color_duration();
    count = _clamp_count(count);

    // If still forced, maintain the forced color
    if (_forced_colour) {
        _fill_colour(_forced_colour_value, count);
        return;
    }

    if (!_leds_enabled) {
        return;
    }

    // Pick a new random color
    Colour current_colour = read_colour_from_list(-1);

    _fill_colour(current_colour, count);
    return;
}

void My::LED::LED::set_colour(const My::LED::Colour &colour, const uint32_t duration, const int16_t count, const My::LED::Colour &background)
{
    int16_t count_clamped = _clamp_count(count);
    _forced_colour = true;
    _forced_colour_value = colour;

    _fill_colour(colour, count_clamped, background);

    _process_timer(duration);
}

void My::LED::LED::set_colour_from_list(const int16_t index, const uint32_t duration, const int16_t count)
{
    Colour colour = read_colour_from_list(index);
    set_colour(colour, duration, count);
}

void My::LED::LED::set_led_position(const uint16_t led_index, const Colour &colour, const uint32_t duration, const bool refresh)
{
    // Clamp indices to valid range
    uint16_t led_index_cleaned = _clamp_index_inclusif(led_index);

    const uint32_t fgPacked = _led_strip.Color(
        colour.r,
        colour.g,
        colour.b,
        colour.w
    );
    // Fill background first
    _led_strip.setPixelColor(led_index_cleaned, fgPacked);
    if (refresh) {
        _led_strip.show();
    }
    _process_timer(duration);
}

void My::LED::LED::set_led_colour(const uint16_t led_index, const Colour &colour, const uint32_t duration, const bool refresh)
{
    set_led_position(led_index, colour, duration, refresh);
}

void My::LED::LED::set_led_position_from_list(const uint16_t led_index, const int16_t colour_index, const uint32_t duration, const bool refresh)
{
    const Colour foreground = read_colour_from_list(colour_index);
    set_led_position(led_index, foreground, duration, refresh);
}

void My::LED::LED::set_colour_from_offset(const uint16_t start_index, const uint16_t end_index, const Colour &foreground, const Colour &background, const uint32_t duration)
{
    _forced_colour = true;
    _forced_colour_value = foreground;

    // Ensure start_index <= end_index
    uint16_t start_index_cleaned = start_index;
    uint16_t end_index_cleaned = end_index;
    if (start_index_cleaned > end_index_cleaned) {
        My::Utils::swap(start_index_cleaned, end_index_cleaned);
    }

    // Clamp indices to valid range
    start_index_cleaned = _clamp_index_inclusif(start_index_cleaned);
    end_index_cleaned = _clamp_index_inclusif(end_index_cleaned);

    const uint32_t fgPacked = _led_strip.Color(
        foreground.r,
        foreground.g,
        foreground.b,
        foreground.w
    );

    const uint32_t bgPacked = _led_strip.Color(
        background.r,
        background.g,
        background.b,
        background.w
    );


    // Fill background first
    for (uint16_t i = 0; i < LED_NUMBER; i++) {
        if (i >= start_index_cleaned && i <= end_index_cleaned) {
            _led_strip.setPixelColor(i, fgPacked);
        } else {
            _led_strip.setPixelColor(i, bgPacked);
        }
    }
    _led_strip.show();
    _process_timer(duration);
}


void My::LED::LED::fancy(My::LED::ColourPos *items, const size_t length, const My::LED::Colour &background, const uint32_t duration)
{
    _forced_colour = true;

    const uint32_t bgPacked = _led_strip.Color(
        background.r,
        background.g,
        background.b,
        background.w
    );

    // 1. Fill background
    for (uint16_t i = 0; i < LED_NUMBER; i++) {
        _led_strip.setPixelColor(i, bgPacked);
    }

    // 2. Apply overlays
    for (size_t i = 0; i < length; i++) {

        // skip inactive items
        if (!items[i].node_enabled) {
            continue;
        }

        // Check that the position is not out of bounds
        const uint16_t pos = items[i].pos;
        if (pos < LED_NUMBER) {
            // Set pixel color
            const Colour &c = items[i].colour;
            _led_strip.setPixelColor(
                pos,
                _led_strip.Color(c.r, c.g, c.b, c.w)
            );
        }

        // Advance position
        _move_pixel(items[i], pos);
    }

    _led_strip.show();

    _process_timer(duration);
}

