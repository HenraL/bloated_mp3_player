#pragma once
#include <Arduino.h>

/* ─── 5×7 bitmap font (ASCII 32–95) ───────────────────────────────── */

#define ASCII_5X7_WIDTH  5
#define ASCII_5X7_HEIGHT 7
#define ASCII_5X7_GAP    6
#define ASCII_5X7_CHARS  64

extern const uint8_t ascii_5x7_bits[][5] PROGMEM;
