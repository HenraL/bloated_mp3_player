#ifndef BAKEDFONTS_MONOFETT_14PT_H_
#define BAKEDFONTS_MONOFETT_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_14PT_WIDTH  10
#define MONOFETT_14PT_HEIGHT 14
#define MONOFETT_14PT_FIRST  32
#define MONOFETT_14PT_LAST   1048575
#define MONOFETT_14PT_COUNT  434

extern const uint32_t monofett_14pt_codes[];

extern const uint8_t monofett_14pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_14pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_14PT_H_ */
