#ifndef BAKEDFONTS_MONOFETT_18PT_H_
#define BAKEDFONTS_MONOFETT_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t MONOFETT_18PT_WIDTH  = 13;
static const uint8_t MONOFETT_18PT_HEIGHT = 36;
static const uint8_t MONOFETT_18PT_ASCENT = 18;
static const uint32_t MONOFETT_18PT_FIRST  = 32;
static const uint32_t MONOFETT_18PT_LAST   = 1048575;
static const uint16_t MONOFETT_18PT_COUNT  = 434;

extern const uint32_t monofett_18pt_codes[];

extern const uint8_t monofett_18pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_18pt_regular_widths[];
extern const FontHandle monofett_18pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_18PT_H_ */
