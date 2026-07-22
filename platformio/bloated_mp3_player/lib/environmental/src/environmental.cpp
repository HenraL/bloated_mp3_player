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
* LAST Modified: 15-07-2026
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

// ═══════════════════════════════════════════════════════════════════════
//  History
// ═══════════════════════════════════════════════════════════════════════

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
    for (size_t i = 0; i < count; i++)
    {
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
    count = 0;
    head = 0;
    temp_min = 999;
    temp_max = -999;
    hum_min = 999;
    hum_max = -999;
    press_min = 99999;
    press_max = 0;
}

// ═══════════════════════════════════════════════════════════════════════
//  Environmental
// ═══════════════════════════════════════════════════════════════════════

Environmental::Environmental::Environmental(
    uint8_t sda, uint8_t scl, uint32_t poll_interval_ms)
    : _sda(sda)
    , _scl(scl)
    , _poll_interval(poll_interval_ms)
    , _has_bmp280(false)
    , _bmp_addr(0)
    , _t_fine(0)
    , _log_interval(2000)
    , _last_log(0)
{
}

// ─── begin ────────────────────────────────────────────────────────────

bool Environmental::Environmental::begin()
{
    Wire.begin(_sda, _scl);

    // --- AHT20 -----------------------------------------------------------
    Wire.beginTransmission(::Environmental::AHT20_ADDR);
    if (Wire.endTransmission() != 0) return false;
    delay(100);

    // --- BMP280 ----------------------------------------------------------
    // Determine which address the BMP280 answers on (0x76 or 0x77)
    _bmp_addr = ::Environmental::BMP280_ADDR;
    Wire.beginTransmission(_bmp_addr);
    _has_bmp280 = (Wire.endTransmission() == 0);
    if (!_has_bmp280)
    {
        _bmp_addr = ::Environmental::BMP280_ADDR_ALT;
        Wire.beginTransmission(_bmp_addr);
        _has_bmp280 = (Wire.endTransmission() == 0);
    }

    if (_has_bmp280)
    {
        // Read calibration data (24 bytes starting at 0x88)
        Wire.beginTransmission(_bmp_addr);
        Wire.write(::Environmental::BMP280_REG_CALIB);
        Wire.endTransmission(false);
        Wire.requestFrom(_bmp_addr, (uint8_t)24);

        if (Wire.available() >= 24)
        {
            _bmp_cal.dig_T1 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_T2 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_T3 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P1 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P2 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P3 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P4 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P5 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P6 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P7 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P8 = Wire.read() | (Wire.read() << 8);
            _bmp_cal.dig_P9 = Wire.read() | (Wire.read() << 8);
        }
        else
        {
            _has_bmp280 = false;
        }

        if (_has_bmp280)
        {
            // Normal mode, 2x temp oversampling, 16x pressure oversampling
            Wire.beginTransmission(_bmp_addr);
            Wire.write(::Environmental::BMP280_REG_CTRL_MEAS);
            Wire.write(0x57);
            Wire.endTransmission();

            // Config: no filter, 0.5 ms standby
            Wire.beginTransmission(_bmp_addr);
            Wire.write(::Environmental::BMP280_REG_CONFIG);
            Wire.write(0x00);
            Wire.endTransmission();
        }
    }

    return true;
}

// ─── read_aht20 ───────────────────────────────────────────────────────

bool Environmental::Environmental::read_aht20(float &temp, float &hum)
{
    // Trigger measurement
    Wire.beginTransmission(::Environmental::AHT20_ADDR);
    Wire.write(::Environmental::AHT20_CMD_TRIGGER);
    Wire.write(::Environmental::AHT20_PARAM_1);
    Wire.write(::Environmental::AHT20_PARAM_2);
    if (Wire.endTransmission() != 0) return false;

    delay(80);

    // Read 6 bytes: status + hum[2] + hum_mid + hum_lo|temp_hi + temp_mid + temp_lo
    Wire.requestFrom(::Environmental::AHT20_ADDR, (uint8_t)6);
    if (Wire.available() < 6) return false;

    uint8_t status     = Wire.read();
    (void)status;
    uint8_t hum_hi     = Wire.read();
    uint8_t hum_mid    = Wire.read();
    uint8_t hum_lo_temp_hi = Wire.read();
    uint8_t temp_mid   = Wire.read();
    uint8_t temp_lo    = Wire.read();

    uint32_t raw_hum = ((uint32_t)hum_hi << 12)
                     | ((uint32_t)hum_mid << 4)
                     | (hum_lo_temp_hi >> 4);
    uint32_t raw_temp = ((uint32_t)(hum_lo_temp_hi & 0x0F) << 16)
                      | ((uint32_t)temp_mid << 8)
                      | temp_lo;

    hum = raw_hum * 100.0f / 0x100000;
    temp = raw_temp * 200.0f / 0x100000 - 50;
    return true;
}

// ─── read_bmp280 ──────────────────────────────────────────────────────

bool Environmental::Environmental::read_bmp280(float &pressure)
{
    // Read 6 bytes: press[3] + temp[3] from 0xF7
    Wire.beginTransmission(_bmp_addr);
    Wire.write(::Environmental::BMP280_REG_DATA);
    Wire.endTransmission(false);
    Wire.requestFrom(_bmp_addr, (uint8_t)6);
    if (Wire.available() < 6) return false;

    uint32_t adc_P = ((uint32_t)Wire.read() << 12)
                   | ((uint32_t)Wire.read() << 4)
                   | (Wire.read() >> 4);
    uint32_t adc_T = ((uint32_t)Wire.read() << 12)
                   | ((uint32_t)Wire.read() << 4)
                   | (Wire.read() >> 4);

    // --- temperature compensation (needed for pressure compensation) ---
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)_bmp_cal.dig_T1 << 1))) * ((int32_t)_bmp_cal.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)_bmp_cal.dig_T1)) * ((adc_T >> 4) - ((int32_t)_bmp_cal.dig_T1))) >> 12) * ((int32_t)_bmp_cal.dig_T3)) >> 14;
    _t_fine = var1 + var2;

    // --- pressure compensation -------------------------------------------
    int64_t var3, var4, p;
    var3 = ((int64_t)_t_fine) - 128000;
    var4 = var3 * var3 * (int64_t)_bmp_cal.dig_P6;
    var4 = var4 + ((var3 * (int64_t)_bmp_cal.dig_P5) << 17);
    var4 = var4 + (((int64_t)_bmp_cal.dig_P4) << 35);
    var3 = ((var3 * var3 * (int64_t)_bmp_cal.dig_P3) >> 8)
         + ((var3 * (int64_t)_bmp_cal.dig_P2) << 12);
    var3 = (((((int64_t)1) << 47) + var3)) * ((int64_t)_bmp_cal.dig_P1) >> 33;

    if (var3 == 0) return false;

    p = 1048576 - adc_P;
    p = (((p << 31) - var4) * 3125) / var3;
    var1 = (((int64_t)_bmp_cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_bmp_cal.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp_cal.dig_P7) << 4);

    pressure = (float)p / 100.0f; // Pa → hPa
    return true;
}

// ─── read ─────────────────────────────────────────────────────────────

bool Environmental::Environmental::read(Reading &out)
{
    out.timestamp = millis();
    bool ok = true;

    float temp = 0, hum = 0, press = 0;

    if (!read_aht20(temp, hum)) ok = false;
    if (_has_bmp280)
    {
        if (!read_bmp280(press)) ok = false;
    }

    if (ok)
    {
        out.temperature = temp;
        out.humidity = hum;
        out.pressure = press;

        uint32_t now = millis();
        if (now - _last_log >= _log_interval)
        {
            _last_log = now;
            _history.push(out);
        }
    }

    return ok;
}

// ─── getters / setters ────────────────────────────────────────────────

uint32_t Environmental::Environmental::get_poll_interval() const
{
    return _poll_interval;
}

Environmental::History &Environmental::Environmental::get_history()
{
    return _history;
}

void Environmental::Environmental::reset_history()
{
    _history.reset();
}

void Environmental::Environmental::set_log_interval(uint32_t ms)
{
    _log_interval = ms;
}

// ═══════════════════════════════════════════════════════════════════════
//  draw_graph  (free function – needs the History reference)
// ═══════════════════════════════════════════════════════════════════════

void Environmental::draw_graph(Screen &display, uint16_t x, uint16_t y,
    uint16_t w, uint16_t h, const History &hist,
    bool show_temp, bool show_hum, bool show_press)
{
    if (hist.count < 2) return;
    size_t plot_count;
    if (hist.count < w) { plot_count = hist.count; } else { plot_count = w; }
    size_t start;
    if (hist.head >= plot_count)
    {
        start = hist.head - plot_count;
    }
    else
    {
        start = hist.count - plot_count;
    }

    float t_range = hist.temp_max - hist.temp_min;
    float h_range = hist.hum_max - hist.hum_min;
    if (t_range < 1) t_range = 1;
    if (h_range < 1) h_range = 1;

    for (size_t i = 0; i < plot_count; i++)
    {
        size_t idx = (start + i) % History::MAX_SAMPLES;
        uint16_t px = x + i * w / plot_count;

        if (show_temp)
        {
            uint16_t py = y + h - ((hist.samples[idx].temperature - hist.temp_min)
                / t_range * (h - 1));
            display.drawPixel(px, py);
        }
        if (show_hum)
        {
            uint16_t py = y + h - ((hist.samples[idx].humidity - hist.hum_min)
                / h_range * (h - 1));
            display.drawPixel(px + 1, py);
        }
    }
}
