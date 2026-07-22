#ifndef BAKEDFONTS_NOTOEMOJI_8PT_H_
#define BAKEDFONTS_NOTOEMOJI_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t NOTOEMOJI_8PT_WIDTH  = 12;
static const uint8_t NOTOEMOJI_8PT_HEIGHT = 15;
static const uint8_t NOTOEMOJI_8PT_ASCENT = 4;
static const uint32_t NOTOEMOJI_8PT_FIRST  = 32;
static const uint32_t NOTOEMOJI_8PT_LAST   = 1042487;
static const uint16_t NOTOEMOJI_8PT_COUNT  = 1501;

extern const uint32_t notoemoji_8pt_codes[];

extern const uint8_t notoemoji_8pt_bits[] PROGMEM;
extern const uint8_t notoemoji_8pt_widths[];
extern const FontHandle notoemoji_8pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_8PT_H_ */
