#ifndef BAKEDFONTS_BITCOUNTSINGLE_20PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_20PT_WIDTH  = 24;
static const uint8_t BITCOUNTSINGLE_20PT_HEIGHT = 38;
static const uint32_t BITCOUNTSINGLE_20PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_20PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_20PT_COUNT  = 396;

extern const uint32_t bitcountsingle_20pt_codes[];

extern const uint8_t bitcountsingle_20pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_20pt_widths[];
extern const FontHandle bitcountsingle_20pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_20PT_H_ */
