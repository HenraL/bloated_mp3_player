#ifndef BAKEDFONTS_BITCOUNTSINGLE_6PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_6PT_WIDTH  = 7;
static const uint8_t BITCOUNTSINGLE_6PT_HEIGHT = 6;
static const uint32_t BITCOUNTSINGLE_6PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_6PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_6PT_COUNT  = 396;

extern const uint32_t bitcountsingle_6pt_codes[];

extern const uint8_t bitcountsingle_6pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_6pt_widths[];
extern const FontHandle bitcountsingle_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_6PT_H_ */
