#ifndef BAKEDFONTS_DOTO_8PT_H_
#define BAKEDFONTS_DOTO_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_8PT_WIDTH  = 5;
static const uint8_t DOTO_8PT_HEIGHT = 16;
static const uint8_t DOTO_8PT_ASCENT = 8;
static const uint32_t DOTO_8PT_FIRST  = 32;
static const uint32_t DOTO_8PT_LAST   = 8722;
static const uint16_t DOTO_8PT_COUNT  = 319;

extern const uint32_t doto_8pt_codes[];

extern const uint8_t doto_8pt_bits[] PROGMEM;
extern const uint8_t doto_8pt_widths[];
extern const FontHandle doto_8pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_8PT_H_ */
