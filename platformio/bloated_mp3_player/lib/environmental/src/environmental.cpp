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
* FILE: environmental.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 1:44:7 17-07-2026
* DESCRIPTION:
* AHT20+BMP280 driver. Because knowing the temperature to three decimal
* places is absolutely essential for playing MP3s at the wrong bitrate.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: I2C environmental sensor implementation.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/environmental.hpp"
#include <Wire.h>

static Environmental::History hist;
static uint32_t log_interval = 2000;
static uint32_t last_log = 0;

void Environmental::History::push(const Reading &r)
{
    samples[head] = r;
    head = (head + 1) % MAX_SAMPLES;
    if (count < MAX_SAMPLES) count++;

    if (r.temperature < temp_min) temp_min = r.temperature;
    if (r.temperature > temp_max) temp_max = r.temperature;
    if (r.humidity < hum_min)     hum_min = r.humidity;
    if (r.humidity > hum_max)     hum_max = r.humidity;
    if (r.pressure < press_min)   press_min = r.pressure;
    if (r.pressure > press_max)   press_max = r.pressure;
}

Environmental::Reading Environmental::History::average() const
{
    Reading avg;
    if (count == 0) return avg;
    for (size_t i = 0; i < count; i++) {
        avg.temperature += samples[i].temperature;
        avg.humidity += samples[i].humidity;
        avg.pressure += samples[i].pressure;
    }
    avg.temperature /= count;
    avg.humidity /= count;
    avg.pressure /= count;
    return avg;
}

void Environmental::History::reset()
{
    count = 0; head = 0;
    temp_min = 999; temp_max = -999;
    hum_min = 999; hum_max = -999;
    press_min = 99999; press_max = 0;
}

bool Environmental::begin(uint8_t sda_pin, uint8_t scl_pin)
{
    Wire.begin(sda_pin, scl_pin);
    Wire.beginTransmission(0x38);
    if (Wire.endTransmission() != 0) return false;
    delay(100);
    return true;
}

bool Environmental::read(Reading &out)
{
    out.timestamp = millis();
    Wire.beginTransmission(0x38);
    Wire.write(0xAC);
    Wire.write(0x33);
    Wire.write(0x00);
    if (Wire.endTransmission() != 0) return false;
    delay(80);

    Wire.requestFrom(0x38, 7);
    if (Wire.available() < 7) return false;
    Wire.read();
    uint32_t raw_hum = ((uint32_t)Wire.read() << 12) |
        ((uint32_t)Wire.read() << 4) |
        (Wire.read() >> 4);
    uint32_t raw_temp = ((uint32_t)Wire.read() << 12) |
        ((uint32_t)Wire.read() << 4) |
        (Wire.read() >> 4);

    out.humidity = raw_hum * 100.0f / 0x100000;
    out.temperature = raw_temp * 200.0f / 0x100000 - 50;

    uint32_t now = millis();
    if (now - last_log >= log_interval) {
        last_log = now;
        hist.push(out);
    }
    return true;
}

bool Environmental::read_async()
{
    Wire.beginTransmission(0x38);
    Wire.write(0xAC);
    Wire.write(0x33);
    Wire.write(0x00);
    return Wire.endTransmission() == 0;
}

bool Environmental::is_data_ready()
{
    Wire.requestFrom(0x38, 1);
    if (Wire.available()) {
        uint8_t status = Wire.read();
        return (status & 0x80) == 0;
    }
    return false;
}

Environmental::History &Environmental::get_history()
{
    return hist;
}

void Environmental::set_log_interval(uint32_t ms)
{
    log_interval = ms;
}

void Environmental::draw_graph(Screen &display, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, bool show_temp,
    bool show_hum, bool show_press)
{
    if (hist.count < 2) return;
    size_t plot_count;
    if (hist.count < w) { plot_count = hist.count; } else { plot_count = w; }
    size_t start = (hist.head >= plot_count)
        ? hist.head - plot_count
        : hist.count - plot_count;

    float t_range = hist.temp_max - hist.temp_min;
    float h_range = hist.hum_max - hist.hum_min;
    if (t_range < 1) t_range = 1;
    if (h_range < 1) h_range = 1;

    for (size_t i = 0; i < plot_count; i++) {
        size_t idx = (start + i) % History::MAX_SAMPLES;
        uint16_t px = x + i * w / plot_count;

        if (show_temp) {
            uint16_t py = y + h - ((hist.samples[idx].temperature - hist.temp_min)
                / t_range * (h - 1));
            display.drawPixel(px, py);
        }
        if (show_hum) {
            uint16_t py = y + h - ((hist.samples[idx].humidity - hist.hum_min)
                / h_range * (h - 1));
            display.drawPixel(px + 1, py);
        }
    }
}
