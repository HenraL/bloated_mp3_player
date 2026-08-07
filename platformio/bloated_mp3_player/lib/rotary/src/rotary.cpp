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
static bool sw_long_reported = false;
static bool sw_was_long_hold = false;
static const uint16_t DEBOUNCE_US = 1000;
static const uint32_t CLICK_WINDOW_MS = 300;
static uint32_t last_release_ms = 0;
static uint8_t click_count = 0;
static uint8_t last_raw_bits = 0xFF;

// When true (default), direction is decoded from both A and B using the
// quadrature transition table; when false, the older single-edge (A-only)
// decode is used. Toggle to compare behaviour on noisy wiring.
static bool quadrature_decode = true;

// When true, the decoded direction is inverted. Wiring-dependent: with the
// pot's A/B swapped (or reversed), the encoder's sense is flipped against
// human logic, so this lets you swap the direction in one line instead of
// rewiring. Default false; set from the application config at init.
static bool direction_inverted = false;

void Rotary::set_quadrature_decode(bool enabled)
{
    quadrature_decode = enabled;
}

void Rotary::set_direction_inverted(bool inverted)
{
    direction_inverted = inverted;
}

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
    if (quadrature_decode) {
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
    } else {
        if (a != last_a && (now - last_a_ms) > DEBOUNCE_US) {
            if (b != a) {
                encoder_pos++;
                last_dir = 1;
            } else {
                encoder_pos--;
                last_dir = -1;
            }
            last_a = a;
            last_b = b;
            last_state = state;
            last_a_ms = now;
        }
    }

    if (sw && !sw_last) {
        // Button released: a press cycle is complete.
        uint32_t now = millis();
        sw_long_reported = false;
        sw_press_ms = 0;
        if (sw_was_long_hold) {
            // Long hold, not a click sequence.
            sw_was_long_hold = false;
            click_count = 0;
            last_release_ms = 0;
        } else if (click_count == 0 || (now - last_release_ms) > CLICK_WINDOW_MS) {
            // First click of a new sequence.
            click_count = 1;
            last_release_ms = now;
        } else {
            // Another click inside the window: count it (cap at 3, anything
            // beyond the third in a single sequence is treated as a comma).
            if (click_count < 3) {
                click_count++;
            }
            last_release_ms = now;
        }
    }
    if (!sw && sw_last) {
        sw_press_ms = millis();
    }
    if (!sw && !sw_last && sw_press_ms > 0 && (millis() - sw_press_ms) > 1000 && !sw_long_reported) {
        sw_long_reported = true;
        sw_was_long_hold = true;
    }
    sw_last = sw;
}

int8_t Rotary::get_direction()
{
    int8_t d = last_dir;
    if (direction_inverted && d != 0)
    {
        d = (int8_t)-d;
    }
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

static uint8_t pending_click_count()
{
    if (click_count == 0) return 0;
    if ((millis() - last_release_ms) <= CLICK_WINDOW_MS) return 0;
    return click_count;
}

static void clear_clicks()
{
    click_count = 0;
    last_release_ms = 0;
}

bool Rotary::was_pressed()
{
    if (pending_click_count() == 1) {
        clear_clicks();
        return true;
    }
    return false;
}

bool Rotary::was_double_pressed()
{
    if (pending_click_count() == 2) {
        clear_clicks();
        return true;
    }
    return false;
}

bool Rotary::was_triple_pressed()
{
    if (pending_click_count() == 3) {
        clear_clicks();
        return true;
    }
    return false;
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
