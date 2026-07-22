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
* FILE: environmental.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 15-07-2026
* DESCRIPTION:
* AHT20+BMP280 combo sensor driver. Measures temperature, humidity, and
* atmospheric pressure. Also measures your patience when the I2C bus
* decides to take a nap.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Read temperature, humidity, pressure with history/graphing.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include <screen.hpp>

#include "constants.hpp"
#include "structures.hpp"


namespace Environmental
{
    class Environmental
    {
    public:
        Environmental(uint8_t sda, uint8_t scl, uint32_t poll_interval_ms);

        bool begin();
        bool read(Reading &out);

        uint32_t get_poll_interval() const;
        bool has_bmp280() const;
        uint8_t get_chip_id() const;
        uint8_t get_bmp_addr() const;
        const BMP280Calibration &get_calibration() const;
        uint32_t get_last_adc_p() const;
        uint32_t get_last_adc_t() const;
        int64_t get_last_intermediate_p() const;

        History &get_history();
        void reset_history();
        void set_log_interval(uint32_t ms);

    private:
        uint8_t  _sda;
        uint8_t  _scl;
        uint32_t _poll_interval;

        BMP280Calibration _bmp_cal;
        bool    _has_bmp280;
        uint8_t _bmp_addr;
        uint8_t _chip_id;
        int32_t _t_fine;

        uint32_t _last_adc_p;
        uint32_t _last_adc_t;
        int64_t  _last_intermediate_p;

        bool read_aht20(float &temp, float &hum);
        bool read_bmp280(float &pressure);

        History  _history;
        uint32_t _log_interval;
        uint32_t _last_log;
    };

    void draw_graph(Screen &display, uint16_t x, uint16_t y,
                    uint16_t w, uint16_t h, const History &hist,
                    bool show_temp = true,
                    bool show_hum = true,
                    bool show_press = false);
}
