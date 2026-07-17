#ifndef BAKEDFONTS_JACQUARD12_10PT_H_
#define BAKEDFONTS_JACQUARD12_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_10PT_WIDTH  14
#define JACQUARD12_10PT_HEIGHT 9
#define JACQUARD12_10PT_FIRST  32
#define JACQUARD12_10PT_LAST   8722
#define JACQUARD12_10PT_COUNT  331

extern const uint16_t jacquard12_10pt_codes[];

extern const uint8_t jacquard12_10pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_10pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_10PT_H_ */
