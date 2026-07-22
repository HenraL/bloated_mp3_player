#ifndef BAKEDFONTS_NOTOEMOJI_2PT_H_
#define BAKEDFONTS_NOTOEMOJI_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t NOTOEMOJI_2PT_WIDTH  = 5;
static const uint8_t NOTOEMOJI_2PT_HEIGHT = 7;
static const uint32_t NOTOEMOJI_2PT_FIRST  = 32;
static const uint32_t NOTOEMOJI_2PT_LAST   = 1042487;
static const uint16_t NOTOEMOJI_2PT_COUNT  = 1501;

extern const uint32_t notoemoji_2pt_codes[];

extern const uint8_t notoemoji_2pt_bits[] PROGMEM;
extern const uint8_t notoemoji_2pt_widths[];
extern const FontHandle notoemoji_2pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_NOTOEMOJI_2PT_H_ */
