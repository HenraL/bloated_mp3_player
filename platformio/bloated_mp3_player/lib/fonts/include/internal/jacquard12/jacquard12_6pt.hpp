#ifndef BAKEDFONTS_JACQUARD12_6PT_H_
#define BAKEDFONTS_JACQUARD12_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_6PT_WIDTH  9
#define JACQUARD12_6PT_HEIGHT 7
#define JACQUARD12_6PT_FIRST  32
#define JACQUARD12_6PT_LAST   8722
#define JACQUARD12_6PT_COUNT  331

extern const uint16_t jacquard12_6pt_codes[];

extern const uint8_t jacquard12_6pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_6pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_6PT_H_ */
