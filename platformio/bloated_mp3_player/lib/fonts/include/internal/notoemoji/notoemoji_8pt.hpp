#ifndef BAKEDFONTS_NOTOEMOJI_8PT_H_
#define BAKEDFONTS_NOTOEMOJI_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_8PT_WIDTH  12
#define NOTOEMOJI_8PT_HEIGHT 11
#define NOTOEMOJI_8PT_FIRST  32
#define NOTOEMOJI_8PT_LAST   1042487
#define NOTOEMOJI_8PT_COUNT  1501

extern const uint32_t notoemoji_8pt_codes[];

extern const uint8_t notoemoji_8pt_bits[] PROGMEM;
extern const uint8_t notoemoji_8pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_8PT_H_ */
