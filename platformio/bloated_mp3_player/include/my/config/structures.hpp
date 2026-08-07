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
* FILE: structures.hpp
* CREATION DATE: 20-07-2026
* LAST Modified: 11:10:57 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: These are the cheerfull structures allowing some elements of the configuration file tu succeed thanks to their smile.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <leds.hpp>
#include <stdint.h>
#include <stddef.h>
namespace My
{
    namespace Config
    {
        namespace Structures
        {
            // ─── Onboard LED ───────────────────────────────────────────────────────
            // Structure for message
            struct LEDStep {
                My::LED::Colour colour;
                uint16_t duration_ms;
            };

            struct LEDMessage {
                const LEDStep *steps;
                const uint16_t length;
            };

            // Type used for the duration of an led colour
            typedef uint16_t duration_ms_t;

            // ─── Vogon panel (second 1602A) ───────────────────────────────
            // A step shows one text frame on the panel for the given dwell.
            // text may point anywhere; the config table below stores PROGMEM.
            struct VogonStep {
                const char *text;
                uint16_t    dwell_ms;
            };

            // A message is a run of steps. The message list is terminated
            // with a {nullptr, 0} entry, same choreography as the LED.
            struct VogonMessage {
                const VogonStep *steps;
                const uint16_t    length;
            };
        } // namespace Structures
    } // namespace Config
} // namespace My
