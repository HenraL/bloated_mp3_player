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
* FILE: vogon_panel.hpp
* CREATION DATE: 07-08-2026
* DESCRIPTION:
* Configuration for the second character LCD: the Vogon panel. Words,
* dwell times, and the quiet little blinking flab that plays between
* messages live here. Nothing is hardcoded in the task.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Vogon panel message timeline configuration.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
#include "structures.hpp"

namespace My
{
    namespace Config
    {
        // ─── Vogon panel (second 1602A) ──────────────────────────────────

        // Comfortable reading pace for a single banner frame.
        static const My::Config::Structures::duration_ms_t vogon_duration_frame = 400;

        // How long a banner sits still once fully revealed on the panel.
        static const My::Config::Structures::duration_ms_t vogon_duration_reveal = 1000;

        // Tempo for the scroll animation between frames.
        static const My::Config::Structures::duration_ms_t vogon_duration_scroll = 240;

        // A "blip" of static shown between two banners. Because even a Vogon
        // deserves a moment of silence.
        static const My::Config::Structures::duration_ms_t vogon_duration_eom = 400;

        // Scrolling speed: chars drifted per scroll tick.
        static const uint8_t vogon_scroll_step = 1;

        // Panel parameters (1602A = 16 columns, 2 rows).
        static const uint8_t vogon_panel_cols = 16;
        static const uint8_t vogon_panel_rows = 2;

        // Every so often the poetry scroll is interrupted so the panel can
        // show the MPU6050 orientation for a few seconds (the sensor values
        // are shared via SharedInstances::imu_orientation). Set to 0 to
        // never swap in the sensor readout.
        static const uint32_t vogon_imu_interval_ms = 15000;
        static const uint32_t vogon_imu_hold_ms = 3000;

        // The banner list. Terminated by a {nullptr, 0} message.
        extern const My::Config::Structures::VogonMessage vogon_messages[] PROGMEM;

        // Short blip played when one banner rolls into the next.
        extern const My::Config::Structures::VogonMessage vogon_eom[] PROGMEM;

    } // namespace Config
} // namespace My