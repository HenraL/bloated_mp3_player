#ifndef BAKEDFONTS_NOTOEMOJI_10PT_H_
#define BAKEDFONTS_NOTOEMOJI_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_10PT_WIDTH  14
#define NOTOEMOJI_10PT_HEIGHT 13
#define NOTOEMOJI_10PT_FIRST  32
#define NOTOEMOJI_10PT_LAST   1042487
#define NOTOEMOJI_10PT_COUNT  1501

extern const uint32_t notoemoji_10pt_codes[];

extern const uint8_t notoemoji_10pt_bits[] PROGMEM;
extern const uint8_t notoemoji_10pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_10PT_H_ */
