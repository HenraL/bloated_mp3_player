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
* FILE: ultrasonic.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Measures distances by screaming at objects and waiting for the echo.
* Like a bat, but with less charisma and more soldering.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: HC-SR04 driver + AirPod-like gesture detection.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/ultrasonic.hpp"

static uint8_t trig, echo;
static Ultrasonic::GestureState gesture;

void Ultrasonic::begin(uint8_t trig_pin, uint8_t echo_pin)
{
    trig = trig_pin;
    echo = echo_pin;
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(trig, LOW);
}

int16_t Ultrasonic::read_cm()
{
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long duration = pulseIn(echo, HIGH, 5000);
    if (duration == 0) return -1;
    return duration * 0.034 / 2;
}

float Ultrasonic::read_inches()
{
    return read_cm() / 2.54;
}

void Ultrasonic::gesture_tick()
{
    static uint32_t last_read = 0;
    uint32_t now = millis();
    if (now - last_read < 30) return;
    last_read = now;

    int16_t d = read_cm();
    if (d < 0) { gesture.swipe_dir = 0; return; }

    now = millis();
    if (now - gesture.last_gesture_ms < gesture.gesture_cooldown_ms)
        return;

    if (d < 10 && !gesture.waiting_for_release)
    {
        gesture.waiting_for_release = true;
        gesture.press_start_cm = d;
        gesture.pressed = true;
        gesture.last_gesture_ms = now;
        return;
    }

    if (d >= 10 && gesture.waiting_for_release)
    {
        gesture.waiting_for_release = false;
        gesture.pressed = false;
        int16_t delta = gesture.press_start_cm - d;
        if (delta > 5)      gesture.swipe_dir = 1;
        else if (delta < -5) gesture.swipe_dir = -1;
        else                 gesture.swipe_dir = 0;
        gesture.last_gesture_ms = now;
        return;
    }

    if (d >= 30)
    {
        gesture.swipe_dir = 0;
        gesture.pressed = false;
    }
}

bool Ultrasonic::is_pressed()
{
    bool p = gesture.pressed;
    gesture.pressed = false;
    return p;
}

int8_t Ultrasonic::get_swipe_dir()
{
    int8_t s = gesture.swipe_dir;
    gesture.swipe_dir = 0;
    return s;
}

void Ultrasonic::set_cooldown(uint16_t ms)
{
    gesture.gesture_cooldown_ms = ms;
}
