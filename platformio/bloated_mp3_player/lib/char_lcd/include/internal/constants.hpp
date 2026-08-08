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
* FILE: constants.hpp
* CREATION DATE: 07-08-2026
* DESCRIPTION:
* Magic numbers for the PCF8574 + HD44780 character LCD.
* Every LCD2024 backpack uses a PCF8574 over I2C. Nothing here is hardcoded
* inline elsewhere; change the values here and the whole house of cards
* re-aligns.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: HD44780 command + backpack bit constants.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>

namespace CharLcd
{
    // PCF8574 output bit assignments (standard I2C backpack mapping).
    // Data pins D7..D4 ride on P7..P4; RS/RW/EN/backlight use P0..P3.
    static const uint8_t LCD_RS_BIT = 0x01;
    static const uint8_t LCD_RW_BIT = 0x02;
    static const uint8_t LCD_EN_BIT = 0x04;
    static const uint8_t LCD_BL_BIT = 0x08;

    // Data registers (mode select for send())
    static const uint8_t MODE_CMD = 0x00;
    static const uint8_t MODE_DATA = 0x01;

    // HD44780 command bytes
    static const uint8_t CMD_CLEAR = 0x01;
    static const uint8_t CMD_HOME = 0x02;
    static const uint8_t CMD_ENTRY_MODE = 0x06;
    static const uint8_t CMD_DISPLAY_CTRL = 0x08;
    static const uint8_t CMD_DISPLAY_ON = 0x04;
    static const uint8_t CMD_CURSOR_OFF = 0x00;
    static const uint8_t CMD_BLINK_OFF = 0x00;
    static const uint8_t CMD_FUNCTION_SET = 0x20;
    static const uint8_t CMD_8BIT = 0x10;
    static const uint8_t CMD_2LINE = 0x08;
    static const uint8_t CMD_5X8 = 0x00;
    static const uint8_t DDRAM_BASE = 0x80;

    // Row start addresses: {row0, row1, row2, row3}
    // 20x4 panels: each line occupies 20 DDRAM slots starting at these.
    // 16x2 panels only ever use row0/row1, so the mapping still holds.
    static const uint8_t ROW_DDRAM[4] = {0x00, 0x40, 0x14, 0x54};

    // Default I2C address for the PCF8574 backpack (A0/A2 tied low is 0x27).
    static const uint8_t DEFAULT_I2C_ADDR = 0x27;

    // Timing (HD44780 datasheet worst cases)
    static const uint16_t INIT_DELAY_US = 4500;      // power-up wake-up
    static const uint16_t INIT_SETTLE_US = 150;      // post-wake settle
    static const uint16_t PULSE_US = 1;              // EN pulse width
    static const uint8_t  EN_HOLD_MS = 0;            // extra EN-low hold (the I2C transactions already provide the inter-byte pause)
    static const uint8_t  CLEAR_HOLD_MS = 2;         // after command/clear
}