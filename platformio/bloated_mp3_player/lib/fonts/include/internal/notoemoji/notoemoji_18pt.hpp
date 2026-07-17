#ifndef BAKEDFONTS_NOTOEMOJI_18PT_H_
#define BAKEDFONTS_NOTOEMOJI_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_18PT_WIDTH  24
#define NOTOEMOJI_18PT_HEIGHT 21
#define NOTOEMOJI_18PT_FIRST  32
#define NOTOEMOJI_18PT_LAST   1042487
#define NOTOEMOJI_18PT_COUNT  1501

extern const uint32_t notoemoji_18pt_codes[];

extern const uint8_t notoemoji_18pt_bits[] PROGMEM;
extern const uint8_t notoemoji_18pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_18PT_H_ */
