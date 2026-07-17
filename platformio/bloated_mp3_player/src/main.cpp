/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* ......................
* ...________________...
* ..|.>_.............|..
* ..|..DON'T........|..
* ..|..PANIC!.......|..
* ..|_______________|..
* ......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: main.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 0:51:49 17-07-2026
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
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include <pins.hpp>
#include <pins_lcd.hpp>

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

#include <Wire.h>

#include <uicommon.hpp>
#include <uilcd.hpp>
#include <uimatrix.hpp>
#include <filexplorer.hpp>
#include <turtle.hpp>

// ─── FreeRTOS task handles ────────────────────────────────────────────
TaskHandle_t ui_task_handle = nullptr;
TaskHandle_t audio_task_handle = nullptr;
TaskHandle_t sensor_task_handle = nullptr;
TaskHandle_t led_task_handle = nullptr;
TaskHandle_t input_task_handle = nullptr;

// ─── Shared state ─────────────────────────────────────────────────────
U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2_lcd(U8G2_R0, LCD_CS, LCD_DC, LCD_RST);
Screen display(u8g2_lcd);
LcdCanvas lcd(display);
MatrixCanvas matrix_cvs;
Adafruit_NeoPixel onboard(1, ONBOARD_LED_PIN, NEO_GRB + NEO_KHZ800);

// ─── Display fonts ────────────────────────────────────────────────────
static const uint8_t* const FONT_TITLE = u8g2_font_ncenB10_tr;
static const uint8_t* const FONT_BODY  = u8g2_font_ncenB08_tr;

// ─── UI Task ──────────────────────────────────────────────────────────
void ui_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWake = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(33);

    while (true) {
        display.clear();
        display.setFont(FONT_TITLE);
        display.printAt("Bloated MP3 v1.0", 0, 12);
        display.setFont(FONT_BODY);

        Environmental::Reading env;
        if (Environmental::read(env)) {
            display.printAt(0, 24, "Temp: %.1f C", env.temperature);
            display.printAt(0, 32, "Hum:  %.0f %%", env.humidity);
            display.printAt(0, 40, "Pres: %.0f hPa", env.pressure);
        }

        display.printAt(0, 50, "Uptime: %lus", millis() / 1000);

        if (Audio::is_playing()) {
            display.printAt(76, 24, ">> PLAY");
        } else {
            display.printAt(76, 24, "|| STOP");
        }

        display.drawRect(0, 54, 128, 8);
        display.fillRect(2, 56, ((millis() / 100) % 100) * 124 / 100, 4);

        display.display();
        vTaskDelayUntil(&xLastWake, freq);
    }
}

// ─── Audio Task ───────────────────────────────────────────────────────
void audio_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWake = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(20);

    while (true) {
        PROFILE_BLOCK("audio_tick");
        Audio::tick();
        vTaskDelayUntil(&xLastWake, freq);
    }
}

// ─── Sensor Task ──────────────────────────────────────────────────────
void sensor_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWake = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(200);

    while (true) {
        PROFILE_BLOCK("sensor_tick");
        Environmental::Reading env;
        Environmental::read(env);

        IMU::Vec3 accel;
        IMU::read_accel(accel);
        IMU::Gesture g = IMU::gesture_tick();

        switch (g) {
            case IMU::Gesture::Shake:
                Audio::stop();
                Audio::play_raw(nullptr, 0);
                break;
            case IMU::Gesture::TiltLeft:
                Audio::pause();
                break;
            case IMU::Gesture::TiltRight:
                Audio::resume();
                break;
            default: break;
        }

        vTaskDelayUntil(&xLastWake, freq);
    }
}

// ─── LED Task ─────────────────────────────────────────────────────────
void led_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWake = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(50);

    while (true) {
        Matrix::tick();

        uint32_t wave = (millis() / 500) % 2;
        onboard.setPixelColor(0,
            onboard.Color(wave ? 0 : 8, wave ? 8 : 0, 0, 0));
        onboard.show();

        vTaskDelayUntil(&xLastWake, freq);
    }
}

// ─── Input Task ───────────────────────────────────────────────────────
void input_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWake = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(10);

    while (true) {
        Rotary::tick();
        Ultrasonic::gesture_tick();

        int8_t dir = Rotary::get_direction();
        if (dir != 0) {
            if (dir > 0)
                Matrix::set_animation(
                    (Matrix::Animation)(((uint8_t)Matrix::get_animation() + 1)
                        % (uint8_t)Matrix::Animation::Count));
            else
                Matrix::set_animation(
                    (Matrix::Animation)(((uint8_t)Matrix::get_animation()
                        - 1 + (uint8_t)Matrix::Animation::Count)
                        % (uint8_t)Matrix::Animation::Count));
        }

        if (Rotary::was_pressed()) {
            if (Audio::is_playing())
                Audio::pause();
            else
                Audio::resume();
        }

        if (Ultrasonic::is_pressed()) {
            Audio::stop();
        }

        int8_t swipe = Ultrasonic::get_swipe_dir();
        if (swipe > 0) {
            if (Audio::is_playing()) {
                Audio::stop();
                // skip track
            }
        }

        vTaskDelayUntil(&xLastWake, freq);
    }
}

// ─── Setup ────────────────────────────────────────────────────────────
void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("Bloated MP3 Player — DON'T PANIC");
    onboard.begin();
    onboard.show();
    onboard.setPixelColor(0, onboard.Color(8, 0, 0));
    onboard.show();

    // SPI bus for LCD
    SPI.begin(LCD_SCLK, IC_SO, LCD_MOSI);
    pinMode(LCD_RST, OUTPUT);
    digitalWrite(LCD_RST, LOW);
    delay(10);
    digitalWrite(LCD_RST, HIGH);
    delay(10);

    // Screen
    display.begin();
    display.clear();
    // Demo: draw a play icon from the generated assets
    display.drawAscii(baseline_play_circle_filled_black_48dp_bits,
        baseline_play_circle_filled_black_48dp_WIDTH,
        baseline_play_circle_filled_black_48dp_HEIGHT, 1, 1);
    display.display();
    delay(500);
    display.clear();
    display.setFont(u8g2_font_ncenB08_tr);
    display.printAt("Booting...", 0, 30);
    display.display();

    // LED matrix
    MY_LED::led_init();
    Matrix::begin(64);
    Matrix::set_animation(Matrix::Animation::Rainbow);

    // I2C sensors
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    // Environmental
    if (!Environmental::begin(I2C_SDA_PIN, I2C_SCL_PIN)) {
        Serial.println("WARNING: AHT20+BMP280 not found");
    }

    // IMU
    if (!IMU::begin(I2C_SDA_PIN, I2C_SCL_PIN)) {
        Serial.println("WARNING: MPU6050 not found");
    }

    // SD card
    if (!SDCard::begin(SD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCLK_PIN)) {
        Serial.println("WARNING: SD card not mounted — check format (FAT32), wiring, or try another card");
    }

    // Audio
    if (!Audio::begin(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DOUT_PIN)) {
        Serial.println("WARNING: I2S init failed");
    }

    // Input devices
    Rotary::begin(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_SW_PIN);
    Ultrasonic::begin(ULTRA_TRIG_PIN, ULTRA_ECHO_PIN);

    // Bluetooth
    Bluetooth::begin("BloatedMP3");

    // ─── Spawn FreeRTOS tasks ─────────────────────────────────────────
    xTaskCreatePinnedToCore(ui_task, "UI", 4096, NULL, 1,
        &ui_task_handle, 1);
    xTaskCreatePinnedToCore(audio_task, "Audio", 2048, NULL, 3,
        &audio_task_handle, 0);
    xTaskCreatePinnedToCore(sensor_task, "Sensors", 2048, NULL, 1,
        &sensor_task_handle, 0);
    xTaskCreatePinnedToCore(led_task, "LEDs", 2048, NULL, 1,
        &led_task_handle, 1);
    xTaskCreatePinnedToCore(input_task, "Input", 2048, NULL, 2,
        &input_task_handle, 0);

    Serial.println("All tasks spawned. Entering the infinite improbability loop.");

    // Dump profiling data on boot (call this interactively in loop() too)
    Profiler::dump_task_stats();
}

void loop()
{
    vTaskDelete(NULL);
}
