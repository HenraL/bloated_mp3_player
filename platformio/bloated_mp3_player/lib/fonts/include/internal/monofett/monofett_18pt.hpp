#ifndef BAKEDFONTS_MONOFETT_18PT_H_
#define BAKEDFONTS_MONOFETT_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_18PT_WIDTH  13
#define MONOFETT_18PT_HEIGHT 18
#define MONOFETT_18PT_FIRST  32
#define MONOFETT_18PT_LAST   1048575
#define MONOFETT_18PT_COUNT  434

extern const uint32_t monofett_18pt_codes[];

extern const uint8_t monofett_18pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_18pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_18PT_H_ */
