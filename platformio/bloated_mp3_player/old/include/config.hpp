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

// Pin configuration
#include "pins.hpp"

// Blink interval (ms)
inline unsigned long blinkInterval = 1000;

// Serial communication baud rate
// NOTE: Use 115200 for hardware Serial to avoid SoftwareSerial interference with BLE at 9600
inline constexpr unsigned long SERIAL_UART_BAUDRATE = 115200;

// Led configs
inline constexpr uint8_t LED_BRIGHTNESS = 200; // 0-255
inline constexpr uint8_t LED_WHITE_LEVEL = 5;  // 0-255
inline constexpr uint16_t LED_NUMBER = 25;//30;    // Number of LEDs in the strip
inline constexpr uint8_t LED_DURATION = 0;   // Duration for color display in setColor functions (0 = infinite)
#ifndef LED_TYPE
#define LED_TYPE NEO_KHZ800 // LED strip type
#endif
#ifndef LED_COLOUR_ORDER
#define LED_COLOUR_ORDER NEO_GRBW // LED strip colour order
#endif

// Led cycle animation
inline constexpr uint32_t LED_CYCLE_INTERVAL_MS = 100; // Interval between frames in cycle animation
inline constexpr int16_t LED_CYCLE_STEP = 1; // Step size for cycle animation

// component led info
inline constexpr int16_t LED_COMPONENT_STEP = 0;
inline constexpr bool LED_COMPONENT_DISABLE_ON_COMPLETE = false;
inline constexpr uint32_t LED_COMPONENT_INTERVAL_MS = 500;

// PDP pseudo-emulation
inline constexpr uint8_t BOTTOM_STRIP_SIZE = 15;
inline constexpr uint8_t TOP_STRIP_START = 15;
inline constexpr uint8_t TOP_STRIP_END = LED_NUMBER - 1; // 29

// Led render timing
inline constexpr unsigned long LED_RENDER_INTERVAL = 100; // Render LEDs every 100ms
