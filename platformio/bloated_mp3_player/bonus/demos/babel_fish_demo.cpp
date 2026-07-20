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
* FILE: babel_fish_demo.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 19:42:37 20-07-2026
* DESCRIPTION:
* Demonstrates the Babel Fish easter egg on both the screen and the
* LED matrix. The fish swims across the LEDs while the screen shows
* the iconic "Don't Panic" message. It's entirely unnecessary, and
* that's exactly the point.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Babel Fish demo / easter egg.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <Arduino.h>
#include <pins.hpp>
#include <pins_lcd.hpp>
#include <screen.hpp>
#include <leds.hpp>
#include <matrix.hpp>

Screen fish_display(LCD_CS, LCD_DC, LCD_RST, LCD_SCLK, LCD_MOSI, IC_SO);

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("Babel Fish — Don't Panic");
    MY::LED::LED led;
    led.led_init();
    Matrix::begin(64);
    Matrix::set_animation(Matrix::Animation::BabelFish);

    fish_display.begin();
    Matrix::babel_fish_splash(fish_display);

    Serial.println("If you can read this, you don't need a Babel Fish.");
    Serial.println("But you have one anyway. Lucky you.");
}

void loop()
{
    Matrix::tick();
    delay(30);
}
