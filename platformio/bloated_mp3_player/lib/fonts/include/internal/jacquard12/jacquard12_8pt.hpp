#ifndef BAKEDFONTS_JACQUARD12_8PT_H_
#define BAKEDFONTS_JACQUARD12_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_8PT_WIDTH  12
#define JACQUARD12_8PT_HEIGHT 7
#define JACQUARD12_8PT_FIRST  32
#define JACQUARD12_8PT_LAST   8722
#define JACQUARD12_8PT_COUNT  331

extern const uint16_t jacquard12_8pt_codes[];

extern const uint8_t jacquard12_8pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_8pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_8PT_H_ */
