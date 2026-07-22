#ifndef BAKEDFONTS_MONOFETT_6PT_H_
#define BAKEDFONTS_MONOFETT_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t MONOFETT_6PT_WIDTH  = 5;
static const uint8_t MONOFETT_6PT_HEIGHT = 7;
static const uint32_t MONOFETT_6PT_FIRST  = 32;
static const uint32_t MONOFETT_6PT_LAST   = 1048575;
static const uint16_t MONOFETT_6PT_COUNT  = 434;

extern const uint32_t monofett_6pt_codes[];

extern const uint8_t monofett_6pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_6pt_regular_widths[];
extern const FontHandle monofett_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_6PT_H_ */
