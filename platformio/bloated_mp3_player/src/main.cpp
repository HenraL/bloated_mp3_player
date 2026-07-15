/*
* +==== BEGIN LCD Screen =================+
* LOGO:
* ......................
* ...________________...
* ..|.>_.............|..
* ..|................|..
* ..|................|..
* ..|________________|..
* ......................
* /STOP
* PROJECT: LCD Screen
* FILE: main.cpp
* CREATION DATE: 07-07-2026
* LAST Modified: 08-07-2026
* DESCRIPTION:
* This is code in charge of driving an lcd screen.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the code in charge of displaying text on an lcd screen.
* // AR
* +==== END LCD Screen =================+
*/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <screen.hpp>
#include <leds.hpp>
#include <pins_lcd.hpp>

#define ONBOARD_PIN 21

Adafruit_NeoPixel onboard(1, ONBOARD_PIN, NEO_GRB + NEO_KHZ800);
Screen screen(LCD_CS, LCD_DC, LCD_RST, LCD_SCLK, LCD_MOSI, IC_SO);

void setup()
{
    Serial.begin(115200);

    onboard.begin();
    onboard.show();

    screen.begin();
    screen.setColumnOffset(4);
    screen.setFont(u8g2_font_ncenB08_tr);
    screen.clear();
    screen.printAt("Hello, World!", 0, 10);
    screen.display();
}

void loop()
{
    onboard.setPixelColor(0, onboard.Color(0, 0, 0, 0));
    onboard.show();
    delay(500);

    static uint8_t count = 0;
    screen.clear();
    screen.setFont(u8g2_font_ncenB08_tr);
    screen.printAt("ESP32-S3 + LCD", 0, 10);
    screen.printAt("GMG12864-06D", 0, 22);
    screen.setFont(u8g2_font_tom_thumb_4x6_tr);
    screen.printAt(0, 34, "Uptime: %lus", millis() / 1000);

    screen.drawRect(0, 54, 128, 8);
    screen.fillRect(2, 56, (count % 100) * 124 / 100, 4);
    count = (count + 1) % 101;

    screen.display();

    onboard.setPixelColor(0, onboard.Color(0, 16, 0, 0));
    onboard.show();
    delay(1000);
}
