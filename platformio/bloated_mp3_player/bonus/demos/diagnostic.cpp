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
* FILE: diagnostic.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 22:54:20 20-07-2026
* DESCRIPTION:
* Quick sanity check for all subsystems. Run this standalone to see
* which components are alive, dead, or merely pretending.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Hardware bring-up test.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include <Arduino.h>
#include <pins.hpp>
#include <pins_lcd.hpp>
#include <Adafruit_NeoPixel.h>
#include <screen.hpp>
#include <leds.hpp>
#include <matrix.hpp>
#include <ultrasonic.hpp>
#include <rotary.hpp>
#include "my/audio.hpp"
#include <environmental.hpp>
#include <imu.hpp>
#include <sdcard.hpp>

Screen diag_display(LCD_CS, LCD_DC, LCD_RST, LCD_SCLK, LCD_MOSI, IC_SO);
Adafruit_NeoPixel diag_led(1, ONBOARD_LED_PIN, NEO_GRB + NEO_KHZ800);

static void diag_print(const char *msg, bool ok)
{
    Serial.print("[ ");
    Serial.print(ok ? "OK" : "FAIL");
    Serial.print(" ] ");
    Serial.println(msg);
    diag_display.printAt(ok ? 0 : 64, diag_display.height() - 12,
        ok ? "OK" : "FAIL");
}

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n=== Bloated MP3 — Diagnostic ===");

    diag_display.begin();
    diag_display.clear();
    diag_display.setFont(u8g2_font_ncenB08_tr);
    diag_display.printAt("Diagnostic", 0, 12);
    diag_display.display();

    diag_led.begin();
    diag_led.setPixelColor(0, diag_led.Color(8, 0, 0));
    diag_led.show();
    diag_print("Onboard LED", true);

    My::LED::LED led;
    led.led_init();
    Matrix::begin(8);
    Matrix::set_animation(Matrix::Animation::Rainbow);
    diag_print("LED Matrix init", true);

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    bool env_ok = Environmental::begin(I2C_SDA_PIN, I2C_SCL_PIN);
    diag_print("AHT20+BMP280", env_ok);

    Environmental::Reading r;
    if (Environmental::read(r)) {
        Serial.printf("  Temp: %.1f C  Hum: %.0f%%  Press: %.0f hPa\n",
            r.temperature, r.humidity, r.pressure);
    }

    bool imu_ok = IMU::begin(I2C_SDA_PIN, I2C_SCL_PIN);
    diag_print("MPU6050", imu_ok);

    if (imu_ok) {
        IMU::Vec3 a;
        IMU::read_accel(a);
        Serial.printf("  Accel: %.2f, %.2f, %.2f\n", a.x, a.y, a.z);
    }

    bool sd_ok = SDCard::begin(SD_CS_PIN, SD_MOSI_PIN,
        SD_MISO_PIN, SD_SCLK_PIN);
    diag_print("SD Card", sd_ok);

    // Audio uses ESP internal DAC on GPIO17/18 — no begin() needed.
    diag_print("Audio (internal DAC)", true);

    Rotary::begin(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_SW_PIN);
    diag_print("Rotary Encoder", true);

    Ultrasonic::begin(ULTRA_TRIG_PIN, ULTRA_ECHO_PIN);
    int16_t dist = Ultrasonic::read_cm();
    Serial.printf("  Ultrasonic: %d cm\n", dist);
    diag_print("Ultrasonic", dist > 0);

    Serial.println("=== Diagnostic complete ===");
    diag_display.setFont(u8g2_font_tom_thumb_4x6_tr);
    diag_display.printAt(0, diag_display.height() - 8, "Done. (c) HF");
    diag_display.display();
}

void loop()
{
    Matrix::tick();
    delay(33);
}
