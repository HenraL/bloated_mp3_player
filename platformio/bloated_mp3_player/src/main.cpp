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
* LAST Modified: 21:18:37 17-07-2026
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
    Serial.begin(My::Config::UART_BAUD);
    My::Serial::initialise();
    My::Threads.serial();
    delay(500);

    // From now on all output goes through the serial queue, so there is
    // no contention: each task sends its message in one atomic
    // xQueueSend call, and the single serial_output_task drains in
    // order.  It also means tasks never block waiting for UART DMA.

    My::Serial::serial_print("Bloated MP3 Player -- DON'T PANIC");
    My::Serial::serial_print("The ships hung in the sky in much the same way that bricks don't.");
    SharedInstances::onboard.begin();
    SharedInstances::onboard.show();
    SharedInstances::onboard.setPixelColor(0, SharedInstances::onboard.Color(8, 0, 0));
    SharedInstances::onboard.show();

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
    MY_LED::led_init();
    Matrix::begin((My::Config::MATRIX_LED_COUNT_HORIZONTAL * My::Config::MATRIX_LED_COUNT_VERTICAL));
    Matrix::set_animation(Matrix::Animation::Rainbow);

    // I2C sensors 
    Wire.begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN);

    // Environmental
    if (!Environmental::begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN)) {
        My::Serial::serial_print("WARN: AHT20+BMP280 -- the answer is 42, but the sensor is 0. Gone where the Vogons would send a badly-written poem.");
    }

    // IMU
    if (!IMU::begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN)) {
        My::Serial::serial_print("WARN: MPU6050 -- we apologize for the inconvenience.");
    }

    // SD card
    if (!SDCard::begin(My::Config::Pins::SD_CS_PIN, My::Config::Pins::SD_MOSI_PIN, My::Config::Pins::SD_MISO_PIN, My::Config::Pins::SD_SCLK_PIN)) {
        My::Serial::serial_print("WARN: SD card -- a common mistake that people make when trying to design something completely foolproof is to underestimate the ingenuity of complete fools.");
    }

    // Audio
    if (!Audio::begin(My::Config::Pins::I2S_BCLK_PIN, My::Config::Pins::I2S_LRC_PIN, My::Config::Pins::I2S_DOUT_PIN)) {
        My::Serial::serial_print("WARN: I2S -- in the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.");
    }

    // Input devices
    Rotary::begin(My::Config::Pins::ROTARY_PIN_A, My::Config::Pins::ROTARY_PIN_B, My::Config::Pins::ROTARY_SW_PIN);
    Ultrasonic::begin(My::Config::Pins::ULTRA_TRIG_PIN, My::Config::Pins::ULTRA_ECHO_PIN);

    // Bluetooth
    Bluetooth::begin("BloatedMP3");

    // ─── Spawn FreeRTOS tasks ─────────────────────────────────────────
    My::Threads::initialise_ui();
    My::Threads::initialise_audio();
    My::Threads::initialise_sensors();
    My::Threads::initialise_led();
    My::Threads::initialise_matrix();
    My::Threads::initialise_input();
    // xTaskCreatePinnedToCore(ui_task, "UI", 4096, NULL, 1,
    //     &ui_task_handle, 1);
    // xTaskCreatePinnedToCore(audio_task, "Audio", 2048, NULL, 3,
    //     &audio_task_handle, 0);
    // xTaskCreatePinnedToCore(sensor_task, "Sensors", 2048, NULL, 1,
    //     &sensor_task_handle, 0);
    // xTaskCreatePinnedToCore(led_task, "LEDs", 2048, NULL, 1,
    //     &led_task_handle, 1);
    // xTaskCreatePinnedToCore(input_task, "Input", 2048, NULL, 2,
    //     &input_task_handle, 0);

    My::Serial.serial_print("All tasks spawned. Entering the infinite improbability loop.");
    Profiler::dump_task_stats();
}

void loop()
{
    // Kill the arduino loop.
    vTaskDelete(NULL);
}
