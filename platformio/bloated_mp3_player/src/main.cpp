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
* LAST Modified: 18:32:46 22-07-2026
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

#include <uimatrix.hpp>
#include <filexplorer.hpp>
#include <turtle.hpp>

// program level includes
#include "my.hpp"
#include "shared_instances.hpp"

void boot_screen()
{
    // Screen
    SharedInstances::lcd.begin();
    SharedInstances::lcd.clear();
    // Demo: draw a play icon from the generated assets
    SharedInstances::lcd.drawAscii(baseline_play_circle_filled_black_48dp_bits,
        baseline_play_circle_filled_black_48dp_WIDTH,
        baseline_play_circle_filled_black_48dp_HEIGHT, 1, 1);
    SharedInstances::lcd.display();
    delay(500);
    SharedInstances::lcd.clear();
    SharedInstances::lcd.setFont(u8g2_font_ncenB08_tr);
    SharedInstances::lcd.printAt("Booting...", 0, 30);
    SharedInstances::lcd.display();
}

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

    // Onboard LED – init first, then colour
    SharedInstances::onboard.init();
    SharedInstances::onboard.set_colour(My::LED::blue_colour, 0, -1, My::LED::black_colour);
    SharedInstances::onboard.refresh();

    // LCD Screen
    SharedInstances::lcd.initialise();
    boot_screen();

    // LED matrix
    Matrix::begin(
        (My::Config::MATRIX_LED_COUNT_HORIZONTAL * My::Config::MATRIX_LED_COUNT_VERTICAL),
        My::Config::MATRIX_LED_COUNT_HORIZONTAL,
        My::Config::Pins::MATRIX_PIN
    );

    // Quick hardware test: light all LEDs white briefly
    for (uint16_t i = 0; i < (My::Config::MATRIX_LED_COUNT_HORIZONTAL * My::Config::MATRIX_LED_COUNT_VERTICAL); i++) {
        Matrix::set_pixel(i, My::LED::blue_colour);
    }
    Matrix::show();
    delay(2100);

    Matrix::set_animation(Matrix::Animation::Rainbow);

    // I2C sensors 
    Wire.begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN);

    // Environmental (AHT20+BMP280)
    if (!SharedInstances::environmental.begin()) {
        SharedInstances::serial.serial_print("WARN: AHT20+BMP280 -- the answer is 42, but the sensor is 0. Gone where the Vogons would send a badly-written poem.");
        if (SharedInstances::environmental.get_chip_id() != 0) {
            SharedInstances::serial.serial_print("[ENV] BMP280 chip ID mismatch: got 0x%02X, expected 0x58",
                SharedInstances::environmental.get_chip_id());
        }
    }

    if (SharedInstances::environmental.has_bmp280()) {
        const auto &cal = SharedInstances::environmental.get_calibration();
        SharedInstances::serial.serial_print(
            "[ENV] BMP280 detected at 0x%02X, chip ID=0x%02X",
            SharedInstances::environmental.get_bmp_addr(),
            SharedInstances::environmental.get_chip_id());
        SharedInstances::serial.serial_print(
            "[ENV] BMP280 cal T1=%u T2=%d T3=%d",
            cal.dig_T1, cal.dig_T2, cal.dig_T3);
        SharedInstances::serial.serial_print(
            "[ENV] BMP280 cal P1=%u P2=%d P3=%d P4=%d P5=%d P6=%d P7=%d P8=%d P9=%d",
            cal.dig_P1, cal.dig_P2, cal.dig_P3, cal.dig_P4,
            cal.dig_P5, cal.dig_P6, cal.dig_P7, cal.dig_P8, cal.dig_P9);
    } else {
        SharedInstances::serial.serial_print("[ENV] BMP280 not found");
    }

    // IMU
    if (!IMU::begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN)) {
        SharedInstances::serial.serial_print("WARN: MPU6050 -- we apologize for the inconvenience.");
    }

    // SD card (SDMMC 1-bit mode on hardware pins 38/39/40)
    if (!SDCard::begin(My::Config::Pins::SDMMC_CLK, My::Config::Pins::SDMMC_CMD, My::Config::Pins::SDMMC_D0)) {
        SharedInstances::serial.serial_print("WARN: SD card -- a common mistake that people make when trying to design something completely foolproof is to underestimate the ingenuity of complete fools.");
    }

    // Audio
    if (!SharedInstances::audio.open()) {
        SharedInstances::serial.serial_print("WARN: I2S -- in the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.");
    }

    // Input devices
    Rotary::begin(My::Config::Pins::ROTARY_PIN_A, My::Config::Pins::ROTARY_PIN_B, My::Config::Pins::ROTARY_SW_PIN);
    Ultrasonic::begin(My::Config::Pins::ULTRA_TRIG_PIN, My::Config::Pins::ULTRA_ECHO_PIN);

    // Bluetooth
    Bluetooth::begin("BloatedMP3");

    // Artificial delay (to read the initialisaiton issues)
    delay(My::Config::Delays::PRE_THREAD_INITIALISATION_DELAY);

    // ─── Spawn FreeRTOS tasks ─────────────────────────────────────────
    SharedInstances::my_threads.initialise_ui();
    // SharedInstances::my_threads.initialise_audio();
    SharedInstances::my_threads.initialise_sensors();
    SharedInstances::my_threads.initialise_led();
    // SharedInstances::my_threads.initialise_matrix();
    // SharedInstances::my_threads.initialise_input();

    SharedInstances::serial.serial_print("All tasks spawned. Entering the infinite improbability loop.");
    Profiler::dump_task_stats();
}

void loop()
{
    // Kill the arduino loop.
    vTaskDelete(NULL);
}
