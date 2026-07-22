#ifndef BAKEDFONTS_JACQUARD12_4PT_H_
#define BAKEDFONTS_JACQUARD12_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t JACQUARD12_4PT_WIDTH  = 6;
static const uint8_t JACQUARD12_4PT_HEIGHT = 5;
static const uint32_t JACQUARD12_4PT_FIRST  = 32;
static const uint32_t JACQUARD12_4PT_LAST   = 8722;
static const uint16_t JACQUARD12_4PT_COUNT  = 331;

extern const uint32_t jacquard12_4pt_codes[];

extern const uint8_t jacquard12_4pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_4pt_regular_widths[];
extern const FontHandle jacquard12_4pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_4PT_H_ */
