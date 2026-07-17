#pragma once

#include "font_structs.hpp"
#include "font_constants.hpp"

#include <stdint.h>

namespace BakedFonts { namespace Helpers {

inline uint16_t glyph_index(
    const uint32_t codes[], uint16_t count, uint32_t code_point)
{
    int16_t lo = 0;
    int16_t hi = (int16_t)(count - 1);
    while (lo <= hi) {
        uint16_t mid = (uint16_t)((lo + hi) / 2);
        if (codes[mid] == code_point) return mid;
        if (codes[mid] < code_point) lo = (int16_t)(mid + 1);
        else hi = (int16_t)(mid - 1);
    }
    return 0xFFFF;
}

inline bool glyph_available(
    const uint32_t codes[], uint16_t count,
    const uint8_t widths[], uint32_t code_point)
{
    uint16_t idx = glyph_index(codes, count, code_point);
    if (idx == 0xFFFF) return false;
    return widths[idx] != 0;
}

inline uint8_t glyph_width(
    const uint32_t codes[], uint16_t count,
    const uint8_t widths[], uint32_t code_point)
{
    uint16_t idx = glyph_index(codes, count, code_point);
    if (idx == 0xFFFF) return 0;
    return widths[idx];
}

inline uint16_t style_variant(
    const uint32_t codes[], uint16_t count,
    const FontStyleMap map[],
    uint32_t code_point, StyleIndex style)
{
    uint16_t idx = glyph_index(codes, count, code_point);
    if (idx == 0xFFFF) return STYLE_VARIANT_ABSENT;
    if (style >= STYLE_COUNT) return STYLE_VARIANT_ABSENT;
    switch (style) {
        case STYLE_NORMAL: return map[idx].normal;
        case STYLE_ITALIC: return map[idx].italic;
        case STYLE_BOLD: return map[idx].bold;
        case STYLE_UNDERLINE: return map[idx].underline;
        case STYLE_STRIKETHROUGH: return map[idx].strikethrough;
        default: return STYLE_VARIANT_ABSENT;
    }
}

} }
