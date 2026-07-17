#ifndef BAKEDFONTS_TINY5_16PT_H_
#define BAKEDFONTS_TINY5_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define TINY5_16PT_WIDTH  22
#define TINY5_16PT_HEIGHT 22
#define TINY5_16PT_FIRST  32
#define TINY5_16PT_LAST   43859
#define TINY5_16PT_COUNT  1154

extern const uint16_t tiny5_16pt_codes[];

extern const uint8_t tiny5_16pt_regular_bits[] PROGMEM;
extern const uint8_t tiny5_16pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_TINY5_16PT_H_ */
