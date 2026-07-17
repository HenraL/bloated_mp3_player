#ifndef BAKEDFONTS_JACQUARD12_12PT_H_
#define BAKEDFONTS_JACQUARD12_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_12PT_WIDTH  17
#define JACQUARD12_12PT_HEIGHT 11
#define JACQUARD12_12PT_FIRST  32
#define JACQUARD12_12PT_LAST   8722
#define JACQUARD12_12PT_COUNT  331

extern const uint16_t jacquard12_12pt_codes[];

extern const uint8_t jacquard12_12pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_12pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_12PT_H_ */
