#ifndef BAKEDFONTS_MONOFETT_4PT_H_
#define BAKEDFONTS_MONOFETT_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_4PT_WIDTH  4
#define MONOFETT_4PT_HEIGHT 5
#define MONOFETT_4PT_FIRST  32
#define MONOFETT_4PT_LAST   1048575
#define MONOFETT_4PT_COUNT  434

extern const uint32_t monofett_4pt_codes[];

extern const uint8_t monofett_4pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_4pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_4PT_H_ */
