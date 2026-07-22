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
* FILE: structures.hpp
* CREATION DATE: 22-07-2026
* LAST Modified: 22-07-2026
* DESCRIPTION:
* Data structures used by the environmental sensor library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Reading, History, and BMP280 calibration structs.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>

namespace Environmental
{
    struct Reading
    {
        float temperature = 0;
        float humidity = 0;
        float pressure = 0;
        uint32_t timestamp = 0;
    };

    struct History
    {
        static const size_t MAX_SAMPLES = 256;
        Reading samples[MAX_SAMPLES];
        size_t count = 0;
        size_t head = 0;
        float temp_min = 999, temp_max = -999;
        float hum_min = 999, hum_max = -999;
        float press_min = 99999, press_max = 0;

        void push(const Reading &r);
        Reading average() const;
        void reset();
    };

    struct BMP280Calibration
    {
        uint16_t dig_T1;
        int16_t  dig_T2;
        int16_t  dig_T3;
        uint16_t dig_P1;
        int16_t  dig_P2;
        int16_t  dig_P3;
        int16_t  dig_P4;
        int16_t  dig_P5;
        int16_t  dig_P6;
        int16_t  dig_P7;
        int16_t  dig_P8;
        int16_t  dig_P9;
    };
}
