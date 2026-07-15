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

Adafruit_NeoPixel MY_LED::LedStrip(LED_NUMBER, LED_STRIP_PIN, LED_TYPE + LED_COLOUR_ORDER);
bool MY_LED::LedStripInitialized = false;

// External variables for forced color management
bool MY_LED::forcedColor = false;

// External variables for time in the force colour management
uint32_t MY_LED::forcedColorEndTime = 0; // Time (millis) when forced color should expire

bool MY_LED::ledsEnabled = true;

MY_LED::Colour MY_LED::forcedColourValue;

const MY_LED::Colour MY_LED::white_colour = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::White);
const MY_LED::Colour MY_LED::black_colour = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::Black);
const MY_LED::Colour MY_LED::default_foreground = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::White);
const MY_LED::Colour MY_LED::default_background = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::Black);
const MY_LED::Colour MY_LED::red_colour = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::Red);
const MY_LED::Colour MY_LED::yellow_colour = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::Yellow);
const MY_LED::Colour MY_LED::green_colour = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::Green);
const MY_LED::Colour MY_LED::blue_colour = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::Blue);
const MY_LED::Colour MY_LED::dark_blue = MY_LED::led_get_colour_from_pointer(&MY_LED::Colours::DarkCyan);

void MY_LED::led_init()
{
    if (LedStripInitialized) {
        return;
    }
    LedStrip.begin();             // initialize GPIO / strip
    LedStrip.setBrightness(LED_BRIGHTNESS);
    LedStrip.show();              // clear LEDs
    LedStripInitialized = true;
}

void MY_LED::led_off()
{
    ledsEnabled = false;
    LedStrip.clear();
    LedStrip.show();
}

void MY_LED::led_on()
{
    ledsEnabled = true;
    forcedColor = false;
    LedStrip.begin();
    LedStrip.setBrightness(LED_BRIGHTNESS);
    LedStrip.show();
}

void MY_LED::led_clear()
{
    forcedColor = false;
    LedStrip.clear();
    LedStrip.show();
}

void MY_LED::led_step(int16_t count)
{
    _led_update_forced_color_duration();
    count = _clamp_count(count);

    // If still forced, maintain the forced color
    if (forcedColor) {
        _led_fill_colour(forcedColourValue, count);
        return;
    }

    if (!ledsEnabled) {
        return;
    }

    // Pick a new random color
    Colour currentColour = led_read_colour_from_list(-1);

    _led_fill_colour(currentColour, count);
    return;
}

void MY_LED::led_set_colour(const MY_LED::Colour &colour, const uint32_t duration, const int16_t count, const MY_LED::Colour &background)
{
    int16_t count_clamped = _clamp_count(count);
    forcedColor = true;
    forcedColourValue = colour;

    _led_fill_colour(colour, count_clamped, background);

    _led_process_timer(duration);
}

void MY_LED::led_set_led_position(const uint16_t led_index, const Colour &colour, const uint32_t duration, const bool refresh)
{

    // Clamp indices to valid range
    uint16_t led_index_cleaned = _clamp_index_inclusif(led_index);

    const uint32_t fgPacked = LedStrip.Color(
        colour.r,
        colour.g,
        colour.b,
        colour.w
    );
    // Fill background first
    LedStrip.setPixelColor(led_index_cleaned, fgPacked);
    if (refresh) {
        LedStrip.show();
    }
    _led_process_timer(duration);
}

void MY_LED::led_set_colour_from_offset(const uint16_t start_index, const uint16_t end_index, const Colour &foreground, const Colour &background, const uint32_t duration)
{
    forcedColor = true;
    forcedColourValue = foreground;

    // Ensure start_index <= end_index
    uint16_t start_index_cleaned = start_index;
    uint16_t end_index_cleaned = end_index;
    if (start_index_cleaned > end_index_cleaned) {
        MyUtils::swap(start_index_cleaned, end_index_cleaned);
    }

    // Clamp indices to valid range
    start_index_cleaned = _clamp_index_inclusif(start_index_cleaned);
    end_index_cleaned = _clamp_index_inclusif(end_index_cleaned);

    const uint32_t fgPacked = LedStrip.Color(
        foreground.r,
        foreground.g,
        foreground.b,
        foreground.w
    );

    const uint32_t bgPacked = LedStrip.Color(
        background.r,
        background.g,
        background.b,
        background.w
    );


    // Fill background first
    for (uint16_t i = 0; i < LED_NUMBER; i++) {
        if (i >= start_index_cleaned && i <= end_index_cleaned) {
            LedStrip.setPixelColor(i, fgPacked);
        } else {
            LedStrip.setPixelColor(i, bgPacked);
        }
    }
    LedStrip.show();
    _led_process_timer(duration);
}

void MY_LED::led_fancy(MY_LED::ColourPos *items, const size_t length, const MY_LED::Colour &background, const uint32_t duration)
{
    forcedColor = true;

    const uint32_t bgPacked = LedStrip.Color(
        background.r,
        background.g,
        background.b,
        background.w
    );

    // 1. Fill background
    for (uint16_t i = 0; i < LED_NUMBER; i++) {
        LedStrip.setPixelColor(i, bgPacked);
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
            LedStrip.setPixelColor(
                pos,
                LedStrip.Color(c.r, c.g, c.b, c.w)
            );
        }

        // Advance position
        _move_pixel(items[i], pos);
    }

    LedStrip.show();

    _led_process_timer(duration);
}

