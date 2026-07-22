#ifndef BAKEDFONTS_NOTOEMOJI_6PT_H_
#define BAKEDFONTS_NOTOEMOJI_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t NOTOEMOJI_6PT_WIDTH  = 10;
static const uint8_t NOTOEMOJI_6PT_HEIGHT = 13;
static const uint8_t NOTOEMOJI_6PT_ASCENT = 3;
static const uint32_t NOTOEMOJI_6PT_FIRST  = 32;
static const uint32_t NOTOEMOJI_6PT_LAST   = 1042487;
static const uint16_t NOTOEMOJI_6PT_COUNT  = 1501;

extern const uint32_t notoemoji_6pt_codes[];

extern const uint8_t notoemoji_6pt_bits[] PROGMEM;
extern const uint8_t notoemoji_6pt_widths[];
extern const FontHandle notoemoji_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_6PT_H_ */
