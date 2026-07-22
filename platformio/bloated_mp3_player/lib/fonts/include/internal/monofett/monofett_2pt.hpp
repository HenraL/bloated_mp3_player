#ifndef BAKEDFONTS_MONOFETT_2PT_H_
#define BAKEDFONTS_MONOFETT_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_2PT_WIDTH  2
#define MONOFETT_2PT_HEIGHT 3
#define MONOFETT_2PT_FIRST  32
#define MONOFETT_2PT_LAST   1048575
#define MONOFETT_2PT_COUNT  434

extern const uint32_t monofett_2pt_codes[];

extern const uint8_t monofett_2pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_2pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_2PT_H_ */
