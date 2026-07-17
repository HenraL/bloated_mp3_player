#ifndef BAKEDFONTS_NOTOEMOJI_16PT_H_
#define BAKEDFONTS_NOTOEMOJI_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_16PT_WIDTH  21
#define NOTOEMOJI_16PT_HEIGHT 19
#define NOTOEMOJI_16PT_FIRST  32
#define NOTOEMOJI_16PT_LAST   1042487
#define NOTOEMOJI_16PT_COUNT  1501

extern const uint32_t notoemoji_16pt_codes[];

extern const uint8_t notoemoji_16pt_bits[] PROGMEM;
extern const uint8_t notoemoji_16pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_16PT_H_ */
