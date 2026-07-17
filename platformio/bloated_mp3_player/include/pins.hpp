/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* ......................
* ...________________...
* ..|.>_.............|..
* ..|................|..
* ..|................|..
* ..|________________|..
* ......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: pins.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Central pin assignment header. If the universe ever decides to rearrange
* its wiring, you only need to change things here. The rest of the code will
* carry on as if nothing happened, much like a Vogon constructor fleet.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Master pin definitions for the ESP32-S3 WROOM-1. Change these
* to match your actual wiring. Or don't. I'm a comment, not a cop.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once

// ─── LCD (GMG12864-06D, ST7565R, 128x64 monochrome) ────────────────────
// These are already defined in pins_lcd.hpp; we re-export them here for
// convenience so you only need to touch one file when rewiring the cosmos.
#include "pins_lcd.hpp"

// ─── Onboard addressable LED ───────────────────────────────────────────
// Pin 48 on the ESP32-S3 WROOM-1. The same tiny LED that will blink in
// morse code "DON'T PANIC" when the temperature sensor glitches out.
#define ONBOARD_LED_PIN    48

// ─── I2C bus (AHT20+BMP280 temp / humidity / pressure) ────────────────
// The AHT20+BMP280 combo sits on a standard I2C bus. It is, for all
// intents and purposes, a very small, very polite marmot that tells you
// how warm it is inside your pocket.
#define I2C_SDA_PIN        5
#define I2C_SCL_PIN        15

// ─── SD Card (SPI mode) ────────────────────────────────────────────────
// Because nothing says "bloated" like loading MP3 metadata from a
// microSD card that's smaller than a fingernail.
#define SD_CS_PIN          13
#define SD_MOSI_PIN        11
#define SD_MISO_PIN        12
#define SD_SCLK_PIN        14

// ─── I2S Audio out (to speaker via MAX98357 or similar) ───────────────
// The audio quality will be approximately that of a dalek singing through
// a rolled-up newspaper. This is a feature, not a bug.
#define I2S_BCLK_PIN       16
#define I2S_LRC_PIN        17
#define I2S_DOUT_PIN       18

// ─── Rotary encoder (Alps EC11E, Prusa-style) ─────────────────────────
// Use this to navigate the UI. Turn left to go back, turn right to go
// forward, press to confirm. It is the least confusing thing on this board.
#define ROTARY_PIN_A       1
#define ROTARY_PIN_B       2
#define ROTARY_SW_PIN      3

// ─── Ultrasonic sensor (HC-SR04 or compatible) ────────────────────────
// For detecting when your hand waves in front of the device, as if to
// say "no, really, that's enough." We will copy Apple's AirPod logic,
// which is to say: if it works, call it magic; if it doesn't, call it
// a feature for the next revision.
#define ULTRA_TRIG_PIN     21
#define ULTRA_ECHO_PIN     47

// ─── LED strip / matrix data pin ──────────────────────────────────────
// The RGB matrix uses the same data pin. If you plugged it into something
// else, change this. The LEDs will thank you by not remaining dark.
#ifndef LED_STRIP_PIN
#define LED_STRIP_PIN      48
#endif

// ─── Boot button ───────────────────────────────────────────────────────
// GPIO0 on ESP32-S3, pulled up. Press at boot to enter download mode.
// It's also useful for demonstrating that buttons still work in the
// 21st century.
#define BOOT_BTN_PIN       0
