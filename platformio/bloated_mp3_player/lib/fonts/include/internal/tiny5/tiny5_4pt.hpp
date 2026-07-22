#ifndef BAKEDFONTS_TINY5_4PT_H_
#define BAKEDFONTS_TINY5_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t TINY5_4PT_WIDTH  = 6;
static const uint8_t TINY5_4PT_HEIGHT = 12;
static const uint8_t TINY5_4PT_ASCENT = 4;
static const uint32_t TINY5_4PT_FIRST  = 32;
static const uint32_t TINY5_4PT_LAST   = 43859;
static const uint16_t TINY5_4PT_COUNT  = 1154;

extern const uint32_t tiny5_4pt_codes[];

extern const uint8_t tiny5_4pt_regular_bits[] PROGMEM;
extern const uint8_t tiny5_4pt_regular_widths[];
extern const FontHandle tiny5_4pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_TINY5_4PT_H_ */
