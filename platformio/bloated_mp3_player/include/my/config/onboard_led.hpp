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
* FILE: onboard_led.hpp
* CREATION DATE: 20-07-2026
* LAST Modified: 1:44:40 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that contains the over engineered logic for the single lonely and depressed led that on the esp module.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <leds.hpp>
#include "structures.hpp"

namespace My
{
    namespace Config
    {

        // ─── Onboard LED ───────────────────────────────────────────────────────
        // Initial seed
        static const uint16_t onboard_led_seed = 42;

        // Morse
        // Duration for the off period, when the led is not emiting any light (morse), this is also the baseline for scaling the rest of the morse logic
        static const Config::Structures::duration_ms_t onboard_duration_off = 200;

        // Duration for a dit
        static const Config::Structures::duration_ms_t onboard_duration_dit = onboard_duration_off * 1;

        // Duration for a da
        static const Config::Structures::duration_ms_t onboard_duration_da = onboard_duration_off * 3;

        // Duration between two letters
        static const Config::Structures::duration_ms_t onboard_duration_letter_gap = onboard_duration_off * 3;

        // Duration between two words (a.k.a a space)
        static const Config::Structures::duration_ms_t onboard_duration_word_gap = onboard_duration_off * 7;

        // Generic animation durations

        // Short visual pulse
        static const Config::Structures::duration_ms_t onboard_duration_short = 200;

        // Medium visual pulse
        static const Config::Structures::duration_ms_t onboard_duration_medium = 500;

        // Long visual pulse
        static const Config::Structures::duration_ms_t onboard_duration_long = 1000;

        // Idle/sleep period
        static const Config::Structures::duration_ms_t onboard_duration_idle = 3000;

        // Duration for the end of a message
        static const Config::Structures::duration_ms_t onboard_duration_eom = onboard_duration_off * 10;

        // An easy way to reference the messages if you wish to play a specific one.
        enum onboard_message_id_t {
            ONBOARD_MESSAGE_DON_T_PANIC = 0,
            ONBOARD_MESSAGE_READY,
            ONBOARD_MESSAGE_THINKING,
            ONBOARD_MESSAGE_ERROR,
            ONBOARD_MESSAGE_SLEEP,
            ONBOARD_MESSAGE_42,
        };

        // -.. --- -. .----. -/.--. .- -. .. -.-.//
        extern const My::Config::Structures::LEDMessage onboard_messages[] PROGMEM;

        // This is the animation that is played when one message has finished playing and another one is about to play
        extern const My::Config::Structures::LEDMessage onboard_eom[] PROGMEM;

    } // namespace Config

} // namespace My
