#ifndef BAKEDFONTS_TINY5_14PT_H_
#define BAKEDFONTS_TINY5_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t TINY5_14PT_WIDTH  = 19;
static const uint8_t TINY5_14PT_HEIGHT = 19;
static const uint32_t TINY5_14PT_FIRST  = 32;
static const uint32_t TINY5_14PT_LAST   = 43859;
static const uint16_t TINY5_14PT_COUNT  = 1154;

extern const uint32_t tiny5_14pt_codes[];

extern const uint8_t tiny5_14pt_regular_bits[] PROGMEM;
extern const uint8_t tiny5_14pt_regular_widths[];
extern const FontHandle tiny5_14pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_TINY5_14PT_H_ */
