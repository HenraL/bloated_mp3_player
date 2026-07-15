#pragma once

// GMG12864-06D (ST7565R) 128x64 monochrome LCD
// Wiring: LCD -> ESP32-S3
// VDD    -> 3.3V     VSS -> GND
// A      -> 3.3V (via 100Ω)    K  -> GND
#define LCD_CS   10
#define LCD_RST  9
#define LCD_DC   8
#define LCD_SCLK 7
#define LCD_MOSI 6

// Font ROM on same SPI bus (optional)
#define IC_CS    5
#define IC_SCL   7  // shared with LCD_SCLK
#define IC_SO    4  // MISO
#define IC_S1    6  // shared with LCD_MOSI
