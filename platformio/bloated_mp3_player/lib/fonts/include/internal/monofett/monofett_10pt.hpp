#ifndef BAKEDFONTS_MONOFETT_10PT_H_
#define BAKEDFONTS_MONOFETT_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_10PT_WIDTH  8
#define MONOFETT_10PT_HEIGHT 10
#define MONOFETT_10PT_FIRST  32
#define MONOFETT_10PT_LAST   1048575
#define MONOFETT_10PT_COUNT  434

extern const uint32_t monofett_10pt_codes[];

extern const uint8_t monofett_10pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_10pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_10PT_H_ */
