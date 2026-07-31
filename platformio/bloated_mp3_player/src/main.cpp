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
* LAST Modified: 19:35:28 31-07-2026
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

static void profiling_output(const char *msg)
{
    SharedInstances::serial.serial_print_raw(msg);
}

void boot_screen()
{
    // Screen
    SharedInstances::lcd.begin(My::Config::DISPLAY_CONTRAST);
    SharedInstances::lcd.clear();
    // Demo: draw a play icon from the generated assets
    SharedInstances::lcd.drawAscii(
        baseline_play_circle_filled_black_48dp_bits,
        baseline_play_circle_filled_black_48dp_WIDTH,
        baseline_play_circle_filled_black_48dp_HEIGHT,
        1,
        1
    );
    SharedInstances::lcd.display();
    delay(500);
    SharedInstances::lcd.clear();
    SharedInstances::lcd.setFont(My::Config::FONT_BOOT);
    SharedInstances::lcd.printAt("Booting...", My::Config::DisplayLayout::BOOTING_X, My::Config::DisplayLayout::BOOTING_Y);
    SharedInstances::lcd.display();
}

bool discover_audio_tracks()
{
    if (!SDCard::is_mounted()) {
        SharedInstances::serial.serial_print("WARN: SD card not mounted, not scanning for audio tracks.");
        return false;
    }
    SharedInstances::lcd.clear();
    SharedInstances::lcd.setFont(My::Config::FONT_BOOT);
    SharedInstances::lcd.printAt("Booting...", My::Config::DisplayLayout::BOOTING_X, My::Config::DisplayLayout::BOOTING_Y);
    SharedInstances::lcd.printAt("Discovering music...", My::Config::DisplayLayout::AUDIO_DISCOVERING_X, My::Config::DisplayLayout::AUDIO_DISCOVERING_Y);
    SharedInstances::lcd.display();
    SharedInstances::serial.serial_print("[SD] Scanning for audio tracks...");
    bool ok = SDCard::scan_tracks("/");
    SharedInstances::serial.serial_print("[SD] Found %lu tracks.", SDCard::total_tracks());
    return ok;
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

    // Route profiler output through the queue-based serial
    Profiler::set_output(profiling_output);

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
        delay(My::Config::Delays::ENVIRONMENTAL_INITIALISATION_ISSUE_MS);
    }

    // IMU
    if (!IMU::begin(My::Config::Pins::I2C_SDA_PIN, My::Config::Pins::I2C_SCL_PIN)) {
        SharedInstances::serial.serial_print("WARN: MPU6050 -- we apologize for the inconvenience.");
        delay(My::Config::Delays::IMU_INITIALISATION_FAILURE_MS);
    }

    // SD card (SDMMC 1-bit mode on hardware pins 38/39/40)
    if (!SDCard::begin(My::Config::Pins::SDMMC_CLK, My::Config::Pins::SDMMC_CMD, My::Config::Pins::SDMMC_D0)) {
        SharedInstances::serial.serial_print("WARN: SD card -- a common mistake that people make when trying to design something completely foolproof is to underestimate the ingenuity of complete fools.");
        delay(My::Config::Delays::SD_CARD_NOT_PRESENT_MESSAGE_MS);
    } else {
        if (!discover_audio_tracks()) {
            SharedInstances::serial.serial_print("WARN: No music found -- ");
            delay(My::Config::Delays::SD_CARD_NO_MUSIC_PRESENT_MS);
        }
    }

    // Audio
    if (!SharedInstances::audio.open()) {
        SharedInstances::serial.serial_print("WARN: I2S -- in the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.");
        delay(My::Config::Delays::AUDIO_HANDLER_I2S_OPEN_FAILURE_MS);
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
    SharedInstances::my_threads.initialise_audio();
    SharedInstances::my_threads.initialise_sensors();
    SharedInstances::my_threads.initialise_led();
    // SharedInstances::my_threads.initialise_matrix();
    SharedInstances::my_threads.initialise_input();

    SharedInstances::serial.serial_print("All tasks spawned. Entering the infinite improbability loop.");
    Profiler::dump_task_stats();
}

void loop()
{
    // Kill the arduino loop.
    vTaskDelete(NULL);
}
