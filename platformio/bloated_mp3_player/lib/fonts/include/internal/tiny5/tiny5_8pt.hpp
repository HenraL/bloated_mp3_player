#ifndef BAKEDFONTS_TINY5_8PT_H_
#define BAKEDFONTS_TINY5_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define TINY5_8PT_WIDTH  11
#define TINY5_8PT_HEIGHT 11
#define TINY5_8PT_FIRST  32
#define TINY5_8PT_LAST   43859
#define TINY5_8PT_COUNT  1154

extern const uint16_t tiny5_8pt_codes[];

extern const uint8_t tiny5_8pt_regular_bits[] PROGMEM;
extern const uint8_t tiny5_8pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_TINY5_8PT_H_ */
