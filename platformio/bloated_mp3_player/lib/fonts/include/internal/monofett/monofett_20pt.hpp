#ifndef BAKEDFONTS_MONOFETT_20PT_H_
#define BAKEDFONTS_MONOFETT_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_20PT_WIDTH  14
#define MONOFETT_20PT_HEIGHT 20
#define MONOFETT_20PT_FIRST  32
#define MONOFETT_20PT_LAST   1048575
#define MONOFETT_20PT_COUNT  434

extern const uint32_t monofett_20pt_codes[];

extern const uint8_t monofett_20pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_20pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_20PT_H_ */
