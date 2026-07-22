#ifndef BAKEDFONTS_NOTOEMOJI_4PT_H_
#define BAKEDFONTS_NOTOEMOJI_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_4PT_WIDTH  7
#define NOTOEMOJI_4PT_HEIGHT 7
#define NOTOEMOJI_4PT_FIRST  32
#define NOTOEMOJI_4PT_LAST   1042487
#define NOTOEMOJI_4PT_COUNT  1501

extern const uint32_t notoemoji_4pt_codes[];

extern const uint8_t notoemoji_4pt_bits[] PROGMEM;
extern const uint8_t notoemoji_4pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_4PT_H_ */
