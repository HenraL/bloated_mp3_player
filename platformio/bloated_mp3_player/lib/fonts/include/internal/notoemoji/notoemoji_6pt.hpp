#ifndef BAKEDFONTS_NOTOEMOJI_6PT_H_
#define BAKEDFONTS_NOTOEMOJI_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_6PT_WIDTH  10
#define NOTOEMOJI_6PT_HEIGHT 9
#define NOTOEMOJI_6PT_FIRST  32
#define NOTOEMOJI_6PT_LAST   1042487
#define NOTOEMOJI_6PT_COUNT  1501

extern const uint32_t notoemoji_6pt_codes[];

extern const uint8_t notoemoji_6pt_bits[] PROGMEM;
extern const uint8_t notoemoji_6pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_6PT_H_ */
