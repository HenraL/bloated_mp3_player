#ifndef BAKEDFONTS_BITCOUNTSINGLE_14PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_14PT_WIDTH  = 17;
static const uint8_t BITCOUNTSINGLE_14PT_HEIGHT = 25;
static const uint32_t BITCOUNTSINGLE_14PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_14PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_14PT_COUNT  = 396;

extern const uint32_t bitcountsingle_14pt_codes[];

extern const uint8_t bitcountsingle_14pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_14pt_widths[];
extern const FontHandle bitcountsingle_14pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_14PT_H_ */
