#ifndef BAKEDFONTS_JACQUARD12_20PT_H_
#define BAKEDFONTS_JACQUARD12_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_20PT_WIDTH  28
#define JACQUARD12_20PT_HEIGHT 18
#define JACQUARD12_20PT_FIRST  32
#define JACQUARD12_20PT_LAST   8722
#define JACQUARD12_20PT_COUNT  331

extern const uint16_t jacquard12_20pt_codes[];

extern const uint8_t jacquard12_20pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_20pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_20PT_H_ */
