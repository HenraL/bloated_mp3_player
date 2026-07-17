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
* FILE: ultrasonic.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* HC-SR04 ultrasonic distance sensor driver. Measures how far away your
* hand is so the device can pretend it reads minds. Spoiler: it doesn't.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Distance measurement and gesture recognition.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>

namespace Ultrasonic
{
    struct GestureState
    {
        int16_t last_distance_cm = 0;
        uint32_t last_gesture_ms = 0;
        int8_t swipe_dir = 0;
        bool waiting_for_release = false;
        int16_t press_start_cm = 0;
        bool pressed = false;
        uint16_t gesture_cooldown_ms = 500;
    };

    void begin(uint8_t trig_pin, uint8_t echo_pin);
    int16_t read_cm();
    float read_inches();
    void gesture_tick();
    bool is_pressed();
    int8_t get_swipe_dir();
    void set_cooldown(uint16_t ms);
}
