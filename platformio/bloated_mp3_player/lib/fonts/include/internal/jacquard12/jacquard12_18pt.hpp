#ifndef BAKEDFONTS_JACQUARD12_18PT_H_
#define BAKEDFONTS_JACQUARD12_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_18PT_WIDTH  25
#define JACQUARD12_18PT_HEIGHT 16
#define JACQUARD12_18PT_FIRST  32
#define JACQUARD12_18PT_LAST   8722
#define JACQUARD12_18PT_COUNT  331

extern const uint16_t jacquard12_18pt_codes[];

extern const uint8_t jacquard12_18pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_18pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_18PT_H_ */
