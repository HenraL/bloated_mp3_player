#ifndef BAKEDFONTS_NOTOEMOJI_20PT_H_
#define BAKEDFONTS_NOTOEMOJI_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_20PT_WIDTH  27
#define NOTOEMOJI_20PT_HEIGHT 24
#define NOTOEMOJI_20PT_FIRST  32
#define NOTOEMOJI_20PT_LAST   1042487
#define NOTOEMOJI_20PT_COUNT  1501

extern const uint32_t notoemoji_20pt_codes[];

extern const uint8_t notoemoji_20pt_bits[] PROGMEM;
extern const uint8_t notoemoji_20pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_20PT_H_ */
