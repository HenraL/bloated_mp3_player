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
* FILE: char_lcd.cpp
* CREATION DATE: 07-08-2026
* DESCRIPTION:
* PCF8574-backpack character LCD (HD44780 nibble mode) implementation.
* Speaks the 4-bit protocol over the shared Wire bus. Uses the legacy
* Wire API, so it behaves the same regardless of the sensor library
* installed.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Character I2C LCD driver implementation.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "internal/char_lcd.hpp"

namespace CharLcd
{
    Lcd::Lcd(TwoWire &wire, uint8_t i2c_addr, uint8_t cols, uint8_t rows)
        : _wire(&wire)
        , _addr(i2c_addr)
        , _cols(cols)
        , _rows(rows)
        , _backlight(true)
        , _col(0)
        , _row(0)
        , _display_control(0)
        , _function_set(0)
        , _last_i2c_error(0)
        , _bus_lock(nullptr)
    {
    }

    void Lcd::set_bus_lock(SemaphoreHandle_t mutex)
    {
        _bus_lock = mutex;
    }

    void Lcd::_write_nibble(uint8_t nibble, uint8_t mode)
    {
        // Data rides on P7..P4; mode/rs + en + backlight ride on P3..P0.
        uint8_t data = (nibble & 0x0F) << 4;
        if (mode == CharLcd::MODE_DATA)
        {
            data |= CharLcd::LCD_RS_BIT;
        }
        if (_backlight)
        {
            data |= CharLcd::LCD_BL_BIT;
        }
        _pulse_enable(data);
    }

    void Lcd::_pulse_enable(uint8_t data)
    {
        uint8_t write_error = 0;
        if (_bus_lock != nullptr)
        {
            xSemaphoreTake(_bus_lock, portMAX_DELAY);
        }
        _last_i2c_error = 0;
        _wire->beginTransmission(_addr);
        _wire->write(data | CharLcd::LCD_EN_BIT);
        write_error = _wire->endTransmission();
        delayMicroseconds(CharLcd::PULSE_US);
        if (CharLcd::EN_HOLD_MS > 0)
        {
            delay(CharLcd::EN_HOLD_MS);
        }
        _wire->beginTransmission(_addr);
        _wire->write(data);
        if (write_error == 0)
        {
            write_error = _wire->endTransmission();
        }
        else
        {
            _wire->endTransmission();
        }
        if (_bus_lock != nullptr)
        {
            xSemaphoreGive(_bus_lock);
        }
        if (write_error != 0)
        {
            _last_i2c_error = write_error;
        }
    }

    void Lcd::_send(uint8_t value, uint8_t mode)
    {
        // HD44780 nibble transmission: high nibble first, then low.
        _write_nibble(value >> 4, mode);
        _write_nibble(value & 0x0F, mode);
    }

    void Lcd::_command(uint8_t cmd)
    {
        _send(cmd, CharLcd::MODE_CMD);
        if (cmd == CharLcd::CMD_CLEAR || cmd == CharLcd::CMD_HOME)
        {
            delay(CharLcd::CLEAR_HOLD_MS); // clear/home take longer
        }
    }

    void Lcd::begin()
    {
        _col = 0;
        _row = 0;
        _display_control = CharLcd::CMD_DISPLAY_CTRL | CharLcd::CMD_DISPLAY_ON
            | CharLcd::CMD_CURSOR_OFF | CharLcd::CMD_BLINK_OFF;
        _function_set = CharLcd::CMD_FUNCTION_SET | CharLcd::CMD_8BIT
            | CharLcd::CMD_2LINE | CharLcd::CMD_5X8;

        // Wake-up (datasheet requires at least 40 ms after power-on).
        delay(50);

        // The "just woke up" triple-0x03 dance, falling into 4-bit mode.
        _write_nibble(0x03, CharLcd::MODE_CMD);
        delayMicroseconds(CharLcd::INIT_DELAY_US);
        _write_nibble(0x03, CharLcd::MODE_CMD);
        delayMicroseconds(CharLcd::INIT_DELAY_US);
        _write_nibble(0x03, CharLcd::MODE_CMD);
        delayMicroseconds(CharLcd::INIT_SETTLE_US);
        _write_nibble(0x02, CharLcd::MODE_CMD);
        delayMicroseconds(CharLcd::INIT_SETTLE_US);

        // Now in 4-bit mode: full function-set, display off, clear, on.
        _command(CharLcd::CMD_FUNCTION_SET | CharLcd::CMD_2LINE | CharLcd::CMD_5X8);
        _command(CharLcd::CMD_DISPLAY_CTRL);          // display off
        _command(CharLcd::CMD_CLEAR);
        _command(CharLcd::CMD_ENTRY_MODE);            // increment, no shift
        _command(_display_control);                   // display on, no blink
        set_cursor(0, 0);
    }

    void Lcd::clear()
    {
        _command(CharLcd::CMD_CLEAR);
        _col = 0;
        _row = 0;
    }

    void Lcd::home()
    {
        _command(CharLcd::CMD_HOME);
        _col = 0;
        _row = 0;
    }

    void Lcd::set_cursor(uint8_t col, uint8_t row)
    {
        if (row >= _rows)
        {
            row = (uint8_t)(_rows - 1);
        }
        if (row >= 4)
        {
            row = 0;
        }
        if (col >= _cols)
        {
            col = (uint8_t)(_cols - 1);
        }
        _command(CharLcd::DDRAM_BASE | (CharLcd::ROW_DDRAM[row] + col));
        _col = col;
        _row = row;
    }

    void Lcd::print(const char *s)
    {
        while (s && *s)
        {
            if (_col >= _cols) break;
            _send((uint8_t)*s, CharLcd::MODE_DATA);
            _col++;
            s++;
        }
    }

    void Lcd::printf(const char *fmt, ...)
    {
        char buf[48];
        va_list va;
        va_start(va, fmt);
        vsnprintf(buf, sizeof(buf), fmt, va);
        va_end(va);
        print(buf);
    }

    void Lcd::print_at(uint8_t col, uint8_t row, const char *fmt, ...)
    {
        char buf[64];
        va_list va;
        va_start(va, fmt);
        vsnprintf(buf, sizeof(buf), fmt, va);
        va_end(va);

        set_cursor(col, row);
        print(buf);
    }

    void Lcd::backlight(bool on)
    {
        _backlight = on;
    }
}