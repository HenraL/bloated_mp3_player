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
* FILE: config.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 13:22:18 07-08-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This file contains the configuration used in the program for stability and usage.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once
#include <stdint.h>
#include <U8g2lib.h>
#include <fonts.hpp>
#include "config/pins.hpp"
#include "config/debug.hpp"
#include "config/delays.hpp"
#include "config/priorities.hpp"
#include "config/onboard_led.hpp"
#include "config/display_layout.hpp"


namespace My
{
    namespace Config
    {
        // ─── Serial Baudrate ─────────────────────────────────────────────────────
        static const unsigned long UART_BAUD = 115200;

        // ─── Serial Buffer ───────────────────────────────────────────────────────
#ifndef SERIAL_MSG_LEN
#define SERIAL_MSG_LEN 128
#endif

#ifndef SERIAL_QUEUE_LEN
#define SERIAL_QUEUE_LEN 16
#endif

        // ─── Display fonts ───────────────────────────────────────────────────────
        static const uint8_t *const FONT_BOOT = u8g2_font_ncenB08_tr;
        static const BakedFonts::FontHandle *const FONT_TITLE = &BakedFonts::tiny5_10pt_handle;
        static const BakedFonts::FontHandle *const FONT_BODY = &BakedFonts::tiny5_8pt_handle;
        static const BakedFonts::FontHandle *const FONT_INFO = &BakedFonts::tiny5_8pt_handle;
        static const uint8_t DISPLAY_CONTRAST = 255;
        static const uint32_t DISPLAY_SPI_CLOCK_HZ = 1000000;

        // ─── Matrix Setup ───────────────────────────────────────────────────────
        static const uint16_t MATRIX_LED_COUNT_HORIZONTAL = 16;
        static const uint16_t MATRIX_LED_COUNT_VERTICAL = 16;

        // ─── Audio ─────────────────────────────────────────────────────────────
        static const uint8_t AUDIO_VOLUME_STEP = 4;
        static const uint8_t AUDIO_VOLUME_MAX = 255;
        static const uint8_t AUDIO_VOLUME_MIN = 0;
        static const uint8_t AUDIO_VOLUME_DEFAULT = 100;

        // ─── Character LCD (PCF8574 backpack, HD44780) ─────────────────────────
        // Info panel: 2004A (4 rows x 20 cols). Bridged A0 from the factory
        // state 0x27 down to 0x26 (ties the A0 pad low) so the two panels
        // can share the bus at different addresses.
        static const uint8_t CHAR_LCD_I2C_ADDR = 0x26;
        static const uint8_t CHAR_LCD_COLS = 20;
        static const uint8_t CHAR_LCD_ROWS = 4;
        static const uint32_t CHAR_LCD_REFRESH_MS = 500;

        // Fun panel: 1602A (2 rows x 16 cols) at the shipped 0x27 default
        // (all address pads open). If the two panels come up swapped on
        // the bench, the A0 bridge is on the wrong backpack — move it.
        static const uint8_t CHAR_LCD2_I2C_ADDR = 0x27;
        static const uint8_t CHAR_LCD2_COLS = 16;
        static const uint8_t CHAR_LCD2_ROWS = 2;
        static const uint32_t CHAR_LCD2_REFRESH_MS = 400;

    } // namespace Config
} // namespace My
