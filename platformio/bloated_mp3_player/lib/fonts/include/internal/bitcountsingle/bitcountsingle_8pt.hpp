#ifndef BAKEDFONTS_BITCOUNTSINGLE_8PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_8PT_WIDTH  = 10;
static const uint8_t BITCOUNTSINGLE_8PT_HEIGHT = 8;
static const uint32_t BITCOUNTSINGLE_8PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_8PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_8PT_COUNT  = 396;

extern const uint32_t bitcountsingle_8pt_codes[];

extern const uint8_t bitcountsingle_8pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_8pt_widths[];
extern const FontHandle bitcountsingle_8pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_8PT_H_ */
