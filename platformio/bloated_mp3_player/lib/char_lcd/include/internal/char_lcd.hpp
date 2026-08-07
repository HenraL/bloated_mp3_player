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
* FILE: char_lcd.hpp
* CREATION DATE: 07-08-2026
* DESCRIPTION:
* Driver for HD44780-class character LCDs with a PCF8574 I2C backpack
* (the "cash register" 16x2 / 16x4 alpanumeric displays). Speaks nibble
* mode over I2C, owns nothing global, remembers its own cursor state.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Character I2C LCD driver.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <stdarg.h>

#include "constants.hpp"

namespace CharLcd
{
    class Lcd
    {
        public:
        /**
         * @brief Create a PCF8574-backpack character LCD driver.
         *
         * The backpack I2C address is normally 0x27 (all address pins low) or
         * 0x3F. Cols/rows describe *your* physical panel (16x2, 16x4, …).
         */
        Lcd(TwoWire &wire, uint8_t i2c_addr = CharLcd::DEFAULT_I2C_ADDR,
            uint8_t cols = 16, uint8_t rows = 2);

        /** Initialise the controller into 4-bit nibble mode and clear. */
        void begin();
        void clear();
        void home();

        /** Place the logical cursor at column-based position (0..cols-1). */
        void set_cursor(uint8_t col, uint8_t row);

        /** Print a NUL-terminated string at the cursor (best effort truncate). */
        void print(const char *s);
        void printf(const char *fmt, ...);

        /** Print at a specific row/column (row index is 0-based). */
        void print_at(uint8_t col, uint8_t row, const char *fmt, ...);

        /** Backlight on/off (P3 on the backpack). */
        void backlight(bool on);

        uint8_t cols() const { return _cols; }
        uint8_t rows() const { return _rows; }

        private:
            TwoWire *_wire;
            uint8_t  _addr;
            uint8_t  _cols;
            uint8_t  _rows;
            bool     _backlight;
            uint8_t  _col;
            uint8_t  _row;
            uint8_t  _display_control;
            uint8_t  _function_set;

            void _write_nibble(uint8_t nibble, uint8_t mode);
            void _pulse_enable(uint8_t data);
            void _send(uint8_t value, uint8_t mode);
            void _command(uint8_t cmd);
    };
}