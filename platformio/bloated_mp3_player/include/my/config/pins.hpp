/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* ......................
* ...________________...
* ..|.>_.............|..
* ..|................|..
* ..|................|..
* ..|________________|..
* ......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: pins.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 17:23:7 21-07-2026
* DESCRIPTION:
* Central pin assignment header. If the universe ever decides to rearrange
* its wiring, you only need to change things here. The rest of the code will
* carry on as if nothing happened, much like a Vogon constructor fleet.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Master pin definitions for the ESP32-S3 WROOM-1. Change these
* to match your actual wiring. Or don't. I'm a comment, not a cop.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
namespace My
{
    namespace Config
    {
        namespace Pins
        {
            // ─── LCD (GMG12864-06D, ST7565R, 128x64 monochrome) ────────────────────
            // These are already defined in pins_lcd.hpp; we re-export them here for
            // convenience so you only need to touch one file when rewiring the cosmos.
            // GMG12864-06D (ST7565R) 128x64 monochrome LCD
            // Wiring: LCD -> ESP32-S3
            // VDD    -> 3.3V     VSS -> GND
            // A      -> 3.3V (via 100Ω)    K  -> GND
            static const uint8_t LCD_CS = 10;
            static const uint8_t LCD_RST = 9;  //shown as RSE on the schematic
            static const uint8_t LCD_DC = 8;   //shown as RS on the schematic
            static const uint8_t LCD_SCLK = 7; //shown as SCL on the schematic
            static const uint8_t LCD_MOSI = 6; //shown as SI on the schematic

            // Font ROM on same SPI bus (optional)
            static const uint8_t IC_CS = 5;    //shown as IC_CS on the schematic
            static const uint8_t IC_SCL = 7;   //shared with LCD_SCLK
            static const uint8_t IC_SO = 4;    // MISO
            static const uint8_t IC_S1 = 6;  // shared with LCD_MOSI

            // ─── Onboard addressable LED ───────────────────────────────────────────
            // Pin 48 on the ESP32-S3 WROOM-1. The same tiny LED that will blink in
            // morse code "DON'T PANIC" when the temperature sensor glitches out.
            static const uint8_t ONBOARD_LED_PIN = 48;

            // ─── I2C bus (AHT20+BMP280 temp / humidity / pressure) ────────────────
            // The AHT20+BMP280 combo sits on a standard I2C bus. It is, for all
            // intents and purposes, a very small, very polite marmot that tells you
            // how warm it is inside your pocket.
            static const uint8_t I2C_SDA_PIN = 2;//old: 5; 
            static const uint8_t I2C_SCL_PIN = 1;//old: 15;

            // ─── SD Card (SDMMC 1-bit mode on Freenove hardware pins) ──────────────
            // The card sits on the ESP32‑S3 dedicated SDMMC pins:
            //   IO38 = CMD
            //   IO39 = CLK
            //   IO40 = DAT0
            // No CS pin is needed in SDMMC mode.
            static const uint8_t SDMMC_CLK = 39;
            static const uint8_t SDMMC_CMD = 38;
            static const uint8_t SDMMC_D0  = 40;

            // ─── I2S Audio out (stereo, 2 speakers) ────────────────────────────
            // BCLK (bit clock) and LRC (word select) are shared by both channels.
            // DOUT1 carries left-channel data, DOUT2 carries right-channel data.
            // On the Freenove board, LS1 maps to GPIO18, LS2 maps to GPIO21.
            // The audio quality remains approximately that of a Dalek singing
            // through a rolled-up newspaper. This is a feature, not a bug.
            static const uint8_t I2S_BCLK_PIN = 16;
            static const uint8_t I2S_LRC_PIN = 17;
            static const uint8_t I2S_DOUT_PIN = 18;
            static const uint8_t I2S_DOUT2_PIN = 21;

            // ─── Rotary encoder (Alps EC11E, Prusa-style) ─────────────────────────
            // Use this to navigate the UI. Turn left to go back, turn right to go
            // forward, press to confirm. It is the least confusing thing on this board.
            static const uint8_t ROTARY_PIN_A = 42;//old: 16 (conflicted with I2S BCLK);
            static const uint8_t ROTARY_PIN_B = 15;//old: 2;
            static const uint8_t ROTARY_SW_PIN = 12;//old: 3;

            // ─── Ultrasonic sensor (HC-SR04 or compatible) ────────────────────────
            // For detecting when your hand waves in front of the device, as if to
            // say "no, really, that's enough." We will copy Apple's AirPod logic,
            // which is to say: if it works, call it magic; if it doesn't, call it
            // a feature for the next revision.
            static const uint8_t ULTRA_TRIG_PIN = 14;
            static const uint8_t ULTRA_ECHO_PIN = 13;

            // ─── LED strip / matrix data pin ──────────────────────────────────────
            // The RGB matrix uses the same data pin. If you plugged it into something
            // else, change this. The LEDs will thank you by not remaining dark.
            static const uint8_t MATRIX_PIN = 41;

            // ─── Boot button ───────────────────────────────────────────────────────
            // GPIO0 on ESP32-S3, pulled up. Press at boot to enter download mode.
            // It's also useful for demonstrating that buttons still work in the
            // 21st century.
            static const uint8_t BOOT_BTN_PIN = 0;
        } // namespace Pins
    } // namespace Config
} // namespace My
