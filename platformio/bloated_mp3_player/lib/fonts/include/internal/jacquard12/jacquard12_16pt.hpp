#ifndef BAKEDFONTS_JACQUARD12_16PT_H_
#define BAKEDFONTS_JACQUARD12_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t JACQUARD12_16PT_WIDTH  = 23;
static const uint8_t JACQUARD12_16PT_HEIGHT = 26;
static const uint32_t JACQUARD12_16PT_FIRST  = 32;
static const uint32_t JACQUARD12_16PT_LAST   = 8722;
static const uint16_t JACQUARD12_16PT_COUNT  = 331;

extern const uint32_t jacquard12_16pt_codes[];

extern const uint8_t jacquard12_16pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_16pt_regular_widths[];
extern const FontHandle jacquard12_16pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_16PT_H_ */
