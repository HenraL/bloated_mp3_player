#ifndef BAKEDFONTS_MONOFETT_16PT_H_
#define BAKEDFONTS_MONOFETT_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define MONOFETT_16PT_WIDTH  12
#define MONOFETT_16PT_HEIGHT 16
#define MONOFETT_16PT_FIRST  32
#define MONOFETT_16PT_LAST   1048575
#define MONOFETT_16PT_COUNT  434

extern const uint32_t monofett_16pt_codes[];

extern const uint8_t monofett_16pt_regular_bits[] PROGMEM;
extern const uint8_t monofett_16pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_MONOFETT_16PT_H_ */
