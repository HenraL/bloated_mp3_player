#ifndef BAKEDFONTS_NOTOEMOJI_14PT_H_
#define BAKEDFONTS_NOTOEMOJI_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define NOTOEMOJI_14PT_WIDTH  19
#define NOTOEMOJI_14PT_HEIGHT 17
#define NOTOEMOJI_14PT_FIRST  32
#define NOTOEMOJI_14PT_LAST   1042487
#define NOTOEMOJI_14PT_COUNT  1501

extern const uint32_t notoemoji_14pt_codes[];

extern const uint8_t notoemoji_14pt_bits[] PROGMEM;
extern const uint8_t notoemoji_14pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_14PT_H_ */
