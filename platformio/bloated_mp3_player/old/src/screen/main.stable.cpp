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
* LAST Modified: 0:9:50 08-07-2026
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
#include <U8g2lib.h>
#include <SPI.h>
#include <leds.hpp>
#include <pins_lcd.hpp>

#define ONBOARD_PIN 21

// Column start offset for ERC12864_ALT edge-gap fix (try 0-6)
#define LCD_COL_OFFSET 4

Adafruit_NeoPixel onboard(1, ONBOARD_PIN, NEO_GRB + NEO_KHZ800);

// Uncomment ONE at a time:
//   ERC12864     - full 128x64, try first (may work now with better init)
#define USE_ERC12864_ALT   // works but ~5px shift at edge
// #define USE_64128N
// #define USE_NHD_C12864
// #define USE_ZOLEN_128X64

#if defined(USE_ERC12864)
U8G2_ST7565_ERC12864_F_4W_HW_SPI u8g2(U8G2_R0, LCD_CS, LCD_DC, LCD_RST);
#elif defined(USE_ERC12864_ALT)
U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2(U8G2_R0, LCD_CS, LCD_DC, LCD_RST);
#elif defined(USE_64128N)
U8G2_ST7565_64128N_F_4W_HW_SPI u8g2(U8G2_R0, LCD_CS, LCD_DC, LCD_RST);
#elif defined(USE_NHD_C12864)
U8G2_ST7565_NHD_C12864_F_4W_HW_SPI u8g2(U8G2_R0, LCD_CS, LCD_DC, LCD_RST);
#elif defined(USE_ZOLEN_128X64)
U8G2_ST7565_ZOLEN_128X64_F_4W_HW_SPI u8g2(U8G2_R0, LCD_CS, LCD_DC, LCD_RST);
#endif

void lcd_print_at(const char *text, uint8_t x, uint8_t y)
{
    u8g2.setCursor(x, y);
    u8g2.print(text);
}

void setup()
{
    Serial.begin(115200);

    onboard.begin();
    onboard.show();

    SPI.begin(LCD_SCLK, IC_SO, LCD_MOSI);

    // Manual hardware reset pulse for reliable startup
    pinMode(LCD_RST, OUTPUT);
    digitalWrite(LCD_RST, LOW);
    delay(10);
    digitalWrite(LCD_RST, HIGH);
    delay(10);

    u8g2.begin();

    // Override column start to fix ERC12864_ALT edge gap
    // Change LCD_COL_OFFSET at top of file (0-6) to fill the screen
    digitalWrite(LCD_DC, LOW);
    digitalWrite(LCD_CS, LOW);
    SPI.transfer(0x10 | (LCD_COL_OFFSET >> 4));
    SPI.transfer(0x00 | (LCD_COL_OFFSET & 0x0F));
    digitalWrite(LCD_CS, HIGH);
    digitalWrite(LCD_DC, HIGH);

    u8g2.setContrast(40);  // ST7565R range is 0-63, 40 is a safe starting point

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    lcd_print_at("Hello, World!", 0, 10);
    u8g2.sendBuffer();
}

void loop()
{
    onboard.setPixelColor(0, onboard.Color(0, 0, 0, 0));
    onboard.show();
    delay(500);

    static uint8_t count = 0;
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    lcd_print_at("ESP32-S3 + LCD", 0, 10);
    lcd_print_at("GMG12864-06D", 0, 22);
    u8g2.setFont(u8g2_font_tom_thumb_4x6_tr);
    lcd_print_at("ST7565R 128x64", 0, 34);

    char buf[18];
    snprintf(buf, sizeof(buf), "Uptime: %lus", millis() / 1000);
    lcd_print_at(buf, 0, 47);

    u8g2.drawFrame(0, 54, 128, 8);
    u8g2.drawBox(2, 56, (count % 100) * 124 / 100, 4);
    count = (count + 1) % 101;

    u8g2.sendBuffer();

    onboard.setPixelColor(0, onboard.Color(0, 16, 0, 0));
    onboard.show();
    delay(1000);
}
