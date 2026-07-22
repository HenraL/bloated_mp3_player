#ifndef BAKEDFONTS_BITCOUNTSINGLE_12PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_12PT_WIDTH  = 14;
static const uint8_t BITCOUNTSINGLE_12PT_HEIGHT = 26;
static const uint8_t BITCOUNTSINGLE_12PT_ASCENT = 11;
static const uint32_t BITCOUNTSINGLE_12PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_12PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_12PT_COUNT  = 396;

extern const uint32_t bitcountsingle_12pt_codes[];

extern const uint8_t bitcountsingle_12pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_12pt_widths[];
extern const FontHandle bitcountsingle_12pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_12PT_H_ */
