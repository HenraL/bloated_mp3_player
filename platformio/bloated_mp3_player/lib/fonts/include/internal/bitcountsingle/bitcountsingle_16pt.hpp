#ifndef BAKEDFONTS_BITCOUNTSINGLE_16PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_16PT_WIDTH  = 19;
static const uint8_t BITCOUNTSINGLE_16PT_HEIGHT = 32;
static const uint8_t BITCOUNTSINGLE_16PT_ASCENT = 14;
static const uint32_t BITCOUNTSINGLE_16PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_16PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_16PT_COUNT  = 396;

extern const uint32_t bitcountsingle_16pt_codes[];

extern const uint8_t bitcountsingle_16pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_16pt_widths[];
extern const FontHandle bitcountsingle_16pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_16PT_H_ */
