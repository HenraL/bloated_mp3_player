/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* ......................
* ..________________...
* ..|.>_............|..
* ..|..DON'T........|..
* ..|..PANIC!.......|..
* ..|_______________|..
* ......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: main.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 12:3:38 21-07-2026
* DESCRIPTION:
* The main event loop. Spawns FreeRTOS tasks for every subsystem that
* doesn't absolutely need to run on the same core, and a few that do.
* Like a Vogon ship, everything runs on sheer improbability and
* inadequate power budgets.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Multithreaded chaos coordinator for the bloated MP3 player.
* // AR
* +==== END Bloated MP3 Player =================+
*/
// System required libraries
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// I2C handling
#include <Wire.h>

// onboard libraries (compiled alongside the program)
#include <leds.hpp>
#include <screen.hpp>
#include <matrix.hpp>
#include <ultrasonic.hpp>
#include <rotary.hpp>
#include <audio.hpp>
#include <environmental.hpp>
#include <imu.hpp>
#include <bluetooth.hpp>
#include <sdcard.hpp>
#include <profiling.hpp>
#include <fonts.hpp>
#include <images.hpp>
#include <uicommon.hpp>
#include <uilcd.hpp>
#include <uimatrix.hpp>
#include <filexplorer.hpp>
#include <turtle.hpp>

// program level includes
#include "my.hpp"
#include "shared_instances.hpp"

// ─── Setup ────────────────────────────────────────────────────────────
void setup()
{
    // Initialise serial
    SharedInstances::serial.initialise();
    SharedInstances::my_threads.initialise_serial();
    delay(My::Config::Delays::SERIAL_INITIALISATION_DELAY);

    // From now on all output goes through the serial queue, so there is
    // no contention: each task sends its message in one atomic
    // xQueueSend call, and the single serial_output_task drains in
    // order.  It also means tasks never block waiting for UART DMA.

    // Display boot message
    SharedInstances::serial.serial_print("Bloated MP3 Player -- DON'T PANIC");
    SharedInstances::serial.serial_print("The ships hung in the sky in much the same way that bricks don't.");

    SharedInstances::onboard.set_colour(My::LED::blue_colour, 0, -1, My::LED::black_colour);
    SharedInstances::onboard.refresh();

    // SPI bus for LCD
    SPI.begin(My::Config::Pins::LCD_SCLK, My::Config::Pins::IC_SO, My::Config::Pins::LCD_MOSI);
    pinMode(My::Config::Pins::LCD_RST, OUTPUT);
    digitalWrite(My::Config::Pins::LCD_RST, LOW);
    delay(10);
    digitalWrite(My::Config::Pins::LCD_RST, HIGH);
    delay(10);

    // Screen
    SharedInstances::display.begin();
    SharedInstances::display.clear();
    // Demo: draw a play icon from the generated assets
    SharedInstances::display.drawAscii(baseline_play_circle_filled_black_48dp_bits,
        baseline_play_circle_filled_black_48dp_WIDTH,
        baseline_play_circle_filled_black_48dp_HEIGHT, 1, 1);
    SharedInstances::display.display();
    delay(500);
    SharedInstances::display.clear();
    SharedInstances::display.setFont(u8g2_font_ncenB08_tr);
    SharedInstances::display.printAt("Booting...", 0, 30);
    SharedInstances::display.display();

    // LED matrix
    SharedInstances::onboard.init();
    Matrix::begin(
        (My::Config::MATRIX_LED_COUNT_HORIZONTAL * My::Config::MATRIX_LED_COUNT_VERTICAL),
        My::Config::MATRIX_LED_COUNT_HORIZONTAL,
        My::Config::Pins::MATRIX_PIN
    );
    Matrix::set_animation(Matrix::Animation::Rainbow);

    // I2C sensors 
    Wire.begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN);

    // Environmental
    if (!Environmental::begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN)) {
        SharedInstances::serial.serial_print("WARN: AHT20+BMP280 -- the answer is 42, but the sensor is 0. Gone where the Vogons would send a badly-written poem.");
    }

    // IMU
    if (!IMU::begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN)) {
        SharedInstances::serial.serial_print("WARN: MPU6050 -- we apologize for the inconvenience.");
    }

    // SD card
    if (!SDCard::begin(My::Config::Pins::SD_CS_PIN, My::Config::Pins::SD_MOSI_PIN, My::Config::Pins::SD_MISO_PIN, My::Config::Pins::SD_SCLK_PIN)) {
        SharedInstances::serial.serial_print("WARN: SD card -- a common mistake that people make when trying to design something completely foolproof is to underestimate the ingenuity of complete fools.");
    }

    // Audio
    if (!Audio::begin(My::Config::Pins::I2S_BCLK_PIN, My::Config::Pins::I2S_LRC_PIN, My::Config::Pins::I2S_DOUT_PIN)) {
        SharedInstances::serial.serial_print("WARN: I2S -- in the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.");
    }

    // Input devices
    Rotary::begin(My::Config::Pins::ROTARY_PIN_A, My::Config::Pins::ROTARY_PIN_B, My::Config::Pins::ROTARY_SW_PIN);
    Ultrasonic::begin(My::Config::Pins::ULTRA_TRIG_PIN, My::Config::Pins::ULTRA_ECHO_PIN);

    // Bluetooth
    Bluetooth::begin("BloatedMP3");

    // ─── Spawn FreeRTOS tasks ─────────────────────────────────────────
    SharedInstances::my_threads.initialise_ui();
    SharedInstances::my_threads.initialise_audio();
    SharedInstances::my_threads.initialise_sensors();
    SharedInstances::my_threads.initialise_led();
    SharedInstances::my_threads.initialise_matrix();
    SharedInstances::my_threads.initialise_input();

    SharedInstances::serial.serial_print("All tasks spawned. Entering the infinite improbability loop.");
    Profiler::dump_task_stats();
}

void loop()
{
    // Kill the arduino loop.
    vTaskDelete(NULL);
}
