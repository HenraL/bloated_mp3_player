#ifndef BAKEDFONTS_JACQUARD12_12PT_H_
#define BAKEDFONTS_JACQUARD12_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t JACQUARD12_12PT_WIDTH  = 17;
static const uint8_t JACQUARD12_12PT_HEIGHT = 21;
static const uint32_t JACQUARD12_12PT_FIRST  = 32;
static const uint32_t JACQUARD12_12PT_LAST   = 8722;
static const uint16_t JACQUARD12_12PT_COUNT  = 331;

extern const uint32_t jacquard12_12pt_codes[];

extern const uint8_t jacquard12_12pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_12pt_regular_widths[];
extern const FontHandle jacquard12_12pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_12PT_H_ */
