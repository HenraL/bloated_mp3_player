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
* LAST Modified: 20:39:12 15-07-2026
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
        static constexpr size_t MAX_SAMPLES = 256;
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

    bool begin(uint8_t sda_pin, uint8_t scl_pin);
    bool read(Reading &out);
    bool read_async();
    bool is_data_ready();

    History &get_history();
    void set_log_interval(uint32_t ms);

    void draw_graph(Screen &display, uint16_t x, uint16_t y,
                    uint16_t w, uint16_t h, bool show_temp = true,
                    bool show_hum = true, bool show_press = false);
}
