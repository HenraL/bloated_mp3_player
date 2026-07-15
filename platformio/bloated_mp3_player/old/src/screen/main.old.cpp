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
* CREATION DATE: 22-05-2026
* LAST Modified: 19:56:17 07-07-2026
* DESCRIPTION:
* This is code in charge of driving an lcd screen.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the main section of the code logic.
* // AR
* +==== END LCD Screen =================+
*/


#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <leds.hpp>

#define ONBOARD_PIN 21
#define STRIP_PIN 8

#define STRIP_COUNT 6

Adafruit_NeoPixel onboard(1, ONBOARD_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_COUNT, STRIP_PIN, NEO_GRBW + NEO_KHZ800);

void setOnboard(uint8_t r, uint8_t g, uint8_t b)
{
  onboard.setPixelColor(0, onboard.Color(r, g, b));
  onboard.show();
}

void setStrip(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
  for (int i = 0; i < STRIP_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b, w));
  }
  strip.show();
}

void setup()
{
  onboard.begin();
  strip.begin();

  onboard.show();
  strip.show();

  setOnboard(0, 255, 255);
  setStrip(20, 80, 200, 80);
}

void loop()
{
  setOnboard(255, 0, 0);
  setStrip(255, 0, 0, 0);
  delay(1000);

  setOnboard(0, 255, 0);
  setStrip(0, 255, 0, 0);
  delay(1000);

  setOnboard(0, 0, 255);
  setStrip(0, 0, 255, 0);
  delay(1000);

  setOnboard(255, 255, 255);
  setStrip(0, 0, 0, 255);
  delay(1000);
}
