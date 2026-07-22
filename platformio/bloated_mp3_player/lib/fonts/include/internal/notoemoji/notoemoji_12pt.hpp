#ifndef BAKEDFONTS_NOTOEMOJI_12PT_H_
#define BAKEDFONTS_NOTOEMOJI_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t NOTOEMOJI_12PT_WIDTH  = 17;
static const uint8_t NOTOEMOJI_12PT_HEIGHT = 21;
static const uint32_t NOTOEMOJI_12PT_FIRST  = 32;
static const uint32_t NOTOEMOJI_12PT_LAST   = 1042487;
static const uint16_t NOTOEMOJI_12PT_COUNT  = 1501;

extern const uint32_t notoemoji_12pt_codes[];

extern const uint8_t notoemoji_12pt_bits[] PROGMEM;
extern const uint8_t notoemoji_12pt_widths[];
extern const FontHandle notoemoji_12pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_12PT_H_ */
