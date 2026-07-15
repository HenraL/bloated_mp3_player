/*
* +==== BEGIN CatFeeder =================+
* LOGO:
* ..............(....⁄\
* ...............)..(.')
* ..............(../..)
* ...............\(__)|
* Inspired by Joan Stark
* source https://www.asciiart.eu/
* animals/cats
* /STOP
* PROJECT: CatFeeder
* FILE: config.hpp
* CREATION DATE: 07-02-2026
* LAST Modified: 11:13:0 18-02-2026
* DESCRIPTION:
* This is the project in charge of making the connected cat feeder project work.
* /STOP
* COPYRIGHT: (c) Cat Feeder
* PURPOSE: This is the file in charge of containing all the configuration required for the program to work properly.
* // AR
* +==== END CatFeeder =================+
*/
#pragma once
#include <Adafruit_NeoPixel.h>

// Max values for various types
#include "sentinels.hpp"

// The pin on which the led to control can be found
#ifndef LED_STRIP_PIN
#define LED_STRIP_PIN 1
#endif

// Led configs
#ifndef LED_BRIGHTNESS
#define LED_BRIGHTNESS 200 // 0-255
#endif
#ifndef LED_WHITE_LEVEL
#define LED_WHITE_LEVEL 5  // 0-255
#endif
#ifndef LED_NUMBER
#define LED_NUMBER  25 //30;    // Number of LEDs in the strip
#endif
#ifndef LED_DURATION
#define LED_DURATION 0   // Duration for color display in setColor functions (0 = infinite)
#endif
#ifndef LED_TYPE
#define LED_TYPE NEO_KHZ800 // LED strip type
#endif
#ifndef LED_COLOUR_ORDER
#define LED_COLOUR_ORDER NEO_GRBW // LED strip colour order
#endif

// Led cycle animation
#ifndef LED_CYCLE_INTERVAL_MS
#define LED_CYCLE_INTERVAL_MS  100 // Interval between frames in cycle animation
#endif
#ifndef LED_CYCLE_STEP
#define LED_CYCLE_STEP  1 // Step size for cycle animation
#endif

// component led info
#ifndef LED_COMPONENT_STEP
#define LED_COMPONENT_STEP  0
#endif
#ifndef LED_COMPONENT_DISABLE_ON_COMPLETE
#define LED_COMPONENT_DISABLE_ON_COMPLETE false
#endif
#ifndef LED_COMPONENT_INTERVAL_MS
#define LED_COMPONENT_INTERVAL_MS 500
#endif

