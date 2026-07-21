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
* FILE: animations.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 12:25:38 21-07-2026
* DESCRIPTION:
* Pre-canned animation sequences for the RGB matrix. Includes the
* obligatory Babel Fish easter egg, because no self-respecting
* improbability-driven device should be without one.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Animation engine for the LED matrix. Provides snake, circle,
* pulse, sound-reactive bars, and the legendary Babel Fish.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include <leds.hpp>
#include <screen.hpp>

namespace Matrix
{
    enum class Animation : uint8_t
    {
        Snake,
        Circle,
        Pulse,
        Rainbow,
        SoundBars,
        BabelFish,
        Off,
        Count
    };

    struct AnimationState
    {
        Animation current = Animation::Snake;
        uint32_t last_tick = 0;
        uint16_t tick_interval = 50;
        uint32_t frame = 0;
        uint8_t hue_offset = 0;
        bool active = true;
        uint16_t row_width = 8;
    };

    void begin(uint16_t led_count = 64, uint16_t row_width = 8, uint8_t pin = 3);
    void set_animation(Animation anim);
    Animation get_animation();
    void tick();
    uint16_t led_count();
    uint16_t width();
    uint16_t height();

    void tick_snake();
    void tick_circle();
    void tick_pulse();
    void tick_rainbow();
    void tick_sound_bars(uint8_t *audio_data = nullptr, size_t len = 0);
    void tick_babel_fish();

    void babel_fish_splash(Screen &display);

    void drawAscii(const uint8_t *data, uint16_t w, uint16_t h,
                   uint16_t x, uint16_t y);

    void set_pixel(uint16_t index, My::LED::Colour c);
    void show();
}
