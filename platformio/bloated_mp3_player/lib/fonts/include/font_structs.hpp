#pragma once

#include <stdint.h>

namespace BakedFonts {

struct FontStyleMap {
    uint16_t normal;
    uint16_t italic;
    uint16_t bold;
    uint16_t underline;
    uint16_t strikethrough;
};

struct FontHandle {
    const uint32_t *codes;
    const uint8_t *bits;
    const uint8_t *widths;
    uint16_t count;
    uint8_t glyph_width;
    uint8_t glyph_height;
    uint8_t ascent;
};

}
