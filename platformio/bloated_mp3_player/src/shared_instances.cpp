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
* FILE: shared_instances.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 18:24:59 22-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file in charge of storing the pointers that will be used in the program.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "shared_instances.hpp"
#include "my/config/pins.hpp"
#include <Wire.h>

namespace SharedInstances
{
    SemaphoreHandle_t i2c_bus_lock = nullptr;

    My::TrackBrowser track_browser;
    IMU::Orientation imu_orientation = {0.0f, 0.0f, 0.0f};
    My::LCD::Display lcd;
    MatrixCanvas matrix_cvs;
    My::LED::LED onboard(1, My::Config::Pins::ONBOARD_LED_PIN, NEO_GRB + NEO_KHZ800, My::LED::white_colour, My::LED::black_colour, My::LED::white_colour, 0, 50);
    My::Serial serial(SERIAL_MSG_LEN, SERIAL_QUEUE_LEN, My::Config::UART_BAUD);
    My::Threads my_threads;
    Environmental::Environmental environmental(
        My::Config::Pins::I2C_SDA_PIN,
        My::Config::Pins::I2C_SCL_PIN,
        My::Config::Delays::ENVIRONMENTAL_POLL_INTERVAL_MS
    );
    CharLcd::Lcd char_lcd(
        Wire,
        My::Config::CHAR_LCD_I2C_ADDR,
        My::Config::CHAR_LCD_COLS,
        My::Config::CHAR_LCD_ROWS
    );
    CharLcd::Lcd char_lcd_vogon(
        Wire,
        My::Config::CHAR_LCD2_I2C_ADDR,
        My::Config::CHAR_LCD2_COLS,
        My::Config::CHAR_LCD2_ROWS
    );
    Audio::Audio audio(
        My::Config::Pins::I2S_BCLK_PIN,
        My::Config::Pins::I2S_WS_PIN,
        My::Config::Pins::I2S_DIN_PIN,
        My::Config::Pins::I2S_DIN2_PIN,
        8, 256
    );
    Audio::Player player(audio);
    Bluetooth::Bridge bluetooth;
}
