#ifndef BAKEDFONTS_MONOFETT_8PT_H_
#define BAKEDFONTS_MONOFETT_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t MONOFETT_8PT_WIDTH  = 7;
static const uint8_t MONOFETT_8PT_HEIGHT = 16;
static const uint32_t MONOFETT_8PT_FIRST  = 32;
static const uint32_t MONOFETT_8PT_LAST   = 1048575;
static const uint16_t MONOFETT_8PT_COUNT  = 434;

extern const uint32_t monofett_8pt_codes[];

extern const uint8_t monofett_8pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_8pt_regular_widths[];
extern const FontHandle monofett_8pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_8PT_H_ */
