#ifndef BAKEDFONTS_TINY5_6PT_H_
#define BAKEDFONTS_TINY5_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define TINY5_6PT_WIDTH  8
#define TINY5_6PT_HEIGHT 8
#define TINY5_6PT_FIRST  32
#define TINY5_6PT_LAST   43859
#define TINY5_6PT_COUNT  1154

extern const uint16_t tiny5_6pt_codes[];

extern const uint8_t tiny5_6pt_regular_bits[] PROGMEM;
extern const uint8_t tiny5_6pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_TINY5_6PT_H_ */
