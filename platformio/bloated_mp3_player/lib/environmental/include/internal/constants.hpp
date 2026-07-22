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
* CREATION DATE: 22-07-2026
* LAST Modified: 22-07-2026
* DESCRIPTION:
* I2C addresses and register constants for the AHT20+BMP280 combo sensor.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Hardware constants for the environmental sensor library.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>

namespace Environmental
{
    // ─── AHT20 ────────────────────────────────────────────────────────────
    static const uint8_t AHT20_ADDR         = 0x38;
    static const uint8_t AHT20_CMD_TRIGGER  = 0xAC;
    static const uint8_t AHT20_PARAM_1      = 0x33;
    static const uint8_t AHT20_PARAM_2      = 0x00;

    // ─── BMP280 ───────────────────────────────────────────────────────────
    static const uint8_t BMP280_ADDR        = 0x76;
    static const uint8_t BMP280_ADDR_ALT    = 0x77;
    static const uint8_t BMP280_REG_CALIB   = 0x88;
    static const uint8_t BMP280_REG_CTRL_MEAS = 0xF4;
    static const uint8_t BMP280_REG_CONFIG  = 0xF5;
    static const uint8_t BMP280_REG_DATA    = 0xF7;

    // Normal mode, 2x temp oversampling, 16x pressure oversampling
    static const uint8_t BMP280_CTRL_VALUE  = 0x57;

    // No filter, 0.5 ms standby
    static const uint8_t BMP280_CONFIG_VAL  = 0x00;
}
