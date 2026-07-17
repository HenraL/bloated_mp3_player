#ifndef BAKEDFONTS_TINY5_10PT_H_
#define BAKEDFONTS_TINY5_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define TINY5_10PT_WIDTH  14
#define TINY5_10PT_HEIGHT 14
#define TINY5_10PT_FIRST  32
#define TINY5_10PT_LAST   43859
#define TINY5_10PT_COUNT  1154

extern const uint16_t tiny5_10pt_codes[];

extern const uint8_t tiny5_10pt_regular_bits[] PROGMEM;
extern const uint8_t tiny5_10pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_TINY5_10PT_H_ */
