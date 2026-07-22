#ifndef BAKEDFONTS_MONOFETT_6PT_H_
#define BAKEDFONTS_MONOFETT_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_6PT_WIDTH  5
#define MONOFETT_6PT_HEIGHT 7
#define MONOFETT_6PT_FIRST  32
#define MONOFETT_6PT_LAST   1048575
#define MONOFETT_6PT_COUNT  434

extern const uint32_t monofett_6pt_codes[];

extern const uint8_t monofett_6pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_6pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_6PT_H_ */
