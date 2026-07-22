#ifndef BAKEDFONTS_MONOFETT_12PT_H_
#define BAKEDFONTS_MONOFETT_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t MONOFETT_12PT_WIDTH  = 9;
static const uint8_t MONOFETT_12PT_HEIGHT = 24;
static const uint8_t MONOFETT_12PT_ASCENT = 12;
static const uint32_t MONOFETT_12PT_FIRST  = 32;
static const uint32_t MONOFETT_12PT_LAST   = 1048575;
static const uint16_t MONOFETT_12PT_COUNT  = 434;

extern const uint32_t monofett_12pt_codes[];

extern const uint8_t monofett_12pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_12pt_regular_widths[];
extern const FontHandle monofett_12pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_12PT_H_ */
