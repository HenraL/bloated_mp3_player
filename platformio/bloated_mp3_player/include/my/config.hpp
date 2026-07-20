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
* LAST Modified: 23:40:14 20-07-2026
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
#include "config/pins.hpp"
#include "config/delays.hpp"
#include "config/priorities.hpp"
#include "config/onboard_led.hpp"


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
        static const uint8_t *const FONT_TITLE = u8g2_font_ncenB10_tr;
        static const uint8_t *const FONT_BODY = u8g2_font_ncenB08_tr;

        // ─── Matrix Setup ───────────────────────────────────────────────────────
        static const uint16_t MATRIX_LED_COUNT_HORIZONTAL = 16;
        static const uint16_t MATRIX_LED_COUNT_VERTICAL = 16;

    } // namespace Config
} // namespace My
