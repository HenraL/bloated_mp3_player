#ifndef BAKEDFONTS_NOTOEMOJI_2PT_H_
#define BAKEDFONTS_NOTOEMOJI_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_2PT_WIDTH  5
#define NOTOEMOJI_2PT_HEIGHT 5
#define NOTOEMOJI_2PT_FIRST  32
#define NOTOEMOJI_2PT_LAST   1042487
#define NOTOEMOJI_2PT_COUNT  1501

extern const uint32_t notoemoji_2pt_codes[];

extern const uint8_t notoemoji_2pt_bits[] PROGMEM;
extern const uint8_t notoemoji_2pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_2PT_H_ */
