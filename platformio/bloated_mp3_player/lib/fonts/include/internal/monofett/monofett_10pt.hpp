#ifndef BAKEDFONTS_MONOFETT_10PT_H_
#define BAKEDFONTS_MONOFETT_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t MONOFETT_10PT_WIDTH  = 8;
static const uint8_t MONOFETT_10PT_HEIGHT = 10;
static const uint32_t MONOFETT_10PT_FIRST  = 32;
static const uint32_t MONOFETT_10PT_LAST   = 1048575;
static const uint16_t MONOFETT_10PT_COUNT  = 434;

extern const uint32_t monofett_10pt_codes[];

extern const uint8_t monofett_10pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_10pt_regular_widths[];
extern const FontHandle monofett_10pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_10PT_H_ */
