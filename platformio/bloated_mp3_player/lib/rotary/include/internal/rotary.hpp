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
* FILE: rotary.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Rotary encoder driver (Alps EC11E). Use it to navigate menus, adjust
* volume, or just annoy your flatmate by spinning it rhythmically.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Click-and-twist input handling, Prusa-style.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>

namespace Rotary
{
    void set_quadrature_decode(bool enabled);
    void set_direction_inverted(bool inverted);
    void begin(uint8_t pin_a, uint8_t pin_b, uint8_t pin_sw);
    void tick();
    void read_raw(uint8_t &a, uint8_t &b);
    uint8_t read_sw();
    bool raw_changed(uint8_t &a, uint8_t &b, uint8_t &sw);
    int8_t get_direction();
    bool was_pressed();
    bool was_double_pressed();
    bool was_triple_pressed();
    bool was_long_pressed(uint32_t hold_ms = 1000);
    int16_t get_position();
    void set_position(int16_t pos);
}
