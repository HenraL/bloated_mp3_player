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
* FILE: rotary.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 12:21:22 23-07-2026
* DESCRIPTION:
* Implements a rotary encoder with software debounce and long-press.
* The callback mechanism is minimal because we believe in doing things
* yourself. Like making tea. Or panicking.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Rotary encoder state machine.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/rotary.hpp"

static uint8_t pinA, pinB, pinSW;
static volatile int16_t encoder_pos = 0;
static volatile int8_t last_dir = 0;

static uint8_t last_a = 1, last_b = 1;
static uint8_t last_state = 3;
static uint32_t last_a_ms = 0;
static bool sw_last = true;
static uint32_t sw_press_ms = 0;
static bool sw_was_pressed = false;
static bool sw_long_reported = false;
static const uint16_t DEBOUNCE_US = 1000;
static uint8_t last_raw_bits = 0xFF;

void Rotary::begin(uint8_t a, uint8_t b, uint8_t sw)
{
    pinA = a; pinB = b; pinSW = sw;
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinSW, INPUT_PULLUP);
    last_a = digitalRead(pinA);
    last_b = digitalRead(pinB);
    last_state = (last_a << 1) | last_b;
}

void Rotary::tick()
{
    uint8_t a = digitalRead(pinA);
    uint8_t b = digitalRead(pinB);
    uint32_t now = micros();
    bool sw = digitalRead(pinSW);

    uint8_t state = (a << 1) | b;
    if (state != last_state && (now - last_a_ms) > DEBOUNCE_US) {
        // Quadrature transition table: indexed by (old << 2) | new.
        // Every valid single-step transition maps to +1 or -1; bounces and
        // skipped double-transitions map to 0 and are ignored.
        static const int8_t TRANS[16] = {
            /* 00 -> */  0, -1, 1, 0,
            /* 01 -> */  1,  0, 0, -1,
            /* 10 -> */ -1,  0, 0, 1,
            /* 11 -> */  0,  1, -1, 0,
        };
        int8_t dir = TRANS[(last_state << 2) | state];
        if (dir != 0) {
            encoder_pos += dir;
            last_dir = dir;
        }
        last_state = state;
        last_a_ms = now;
    }

    if (sw && !sw_last) {
        sw_was_pressed = true;
        sw_long_reported = false;
        sw_press_ms = 0;
    }
    if (!sw && sw_last) {
        sw_press_ms = millis();
    }
    if (!sw && !sw_last && sw_press_ms > 0 && (millis() - sw_press_ms) > 1000 && !sw_long_reported) {
        sw_long_reported = true;
    }
    sw_last = sw;
}

int8_t Rotary::get_direction()
{
    int8_t d = last_dir;
    last_dir = 0;
    return d;
}

void Rotary::read_raw(uint8_t &a, uint8_t &b)
{
    a = digitalRead(pinA);
    b = digitalRead(pinB);
}

uint8_t Rotary::read_sw()
{
    return digitalRead(pinSW);
}

bool Rotary::raw_changed(uint8_t &a, uint8_t &b, uint8_t &sw)
{
    a = digitalRead(pinA);
    b = digitalRead(pinB);
    sw = digitalRead(pinSW);
    uint8_t bits = (a << 2) | (b << 1) | (sw & 1);
    if (bits == last_raw_bits) {
        return false;
    }
    last_raw_bits = bits;
    return true;
}

bool Rotary::was_pressed()
{
    bool p = sw_was_pressed;
    sw_was_pressed = false;
    return p;
}

bool Rotary::was_long_pressed(uint32_t hold_ms)
{
    if (!digitalRead(pinSW) && (millis() - sw_press_ms) > hold_ms && !sw_long_reported) {
        sw_long_reported = true;
        return true;
    }
    return false;
}

int16_t Rotary::get_position()
{
    return encoder_pos;
}

void Rotary::set_position(int16_t pos)
{
    encoder_pos = pos;
}
