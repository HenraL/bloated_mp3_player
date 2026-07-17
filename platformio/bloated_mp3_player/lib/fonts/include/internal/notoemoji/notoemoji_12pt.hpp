#ifndef BAKEDFONTS_NOTOEMOJI_12PT_H_
#define BAKEDFONTS_NOTOEMOJI_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_12PT_WIDTH  17
#define NOTOEMOJI_12PT_HEIGHT 15
#define NOTOEMOJI_12PT_FIRST  32
#define NOTOEMOJI_12PT_LAST   1042487
#define NOTOEMOJI_12PT_COUNT  1501

extern const uint32_t notoemoji_12pt_codes[];

extern const uint8_t notoemoji_12pt_bits[] PROGMEM;
extern const uint8_t notoemoji_12pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_12PT_H_ */
