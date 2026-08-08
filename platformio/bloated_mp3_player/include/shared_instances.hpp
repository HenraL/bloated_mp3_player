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
* FILE: shared_instances.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 15:34:24 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file in charge of storing the pointers that will be used in the program.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once
#include <audio.hpp>
#include <char_lcd.hpp>
#include <environmental.hpp>
#include <leds.hpp>
#include <uimatrix.hpp>
#include "my/lcd.hpp"
#include "my/serial.hpp"
#include "my/threads.hpp"

namespace SharedInstances
{
    /**
     * @brief Mutex guarding the shared I2C bus.
     *
     * The PCF8574 LCD backpacks, the AHT20/BMP280 and the MPU6050 all share
     * one Wire bus driven from different FreeRTOS tasks. Created in setup()
     * after Wire.begin(); every task that touches Wire holds this lock.
     */
    extern SemaphoreHandle_t i2c_bus_lock;

    extern My::LCD::Display lcd;
    extern MatrixCanvas matrix_cvs;
    extern My::LED::LED onboard;
    extern My::Serial serial;
    extern My::Threads my_threads;
    extern Environmental::Environmental environmental;
    extern CharLcd::Lcd char_lcd;
    extern CharLcd::Lcd char_lcd_vogon;
    extern Audio::Audio audio;
    extern Audio::Player player;
}
