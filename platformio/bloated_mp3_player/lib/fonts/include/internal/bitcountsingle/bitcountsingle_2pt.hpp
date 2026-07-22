#ifndef BAKEDFONTS_BITCOUNTSINGLE_2PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_2PT_WIDTH  = 3;
static const uint8_t BITCOUNTSINGLE_2PT_HEIGHT = 8;
static const uint32_t BITCOUNTSINGLE_2PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_2PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_2PT_COUNT  = 396;

extern const uint32_t bitcountsingle_2pt_codes[];

extern const uint8_t bitcountsingle_2pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_2pt_widths[];
extern const FontHandle bitcountsingle_2pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_2PT_H_ */
