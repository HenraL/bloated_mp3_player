#ifndef BAKEDFONTS_DOTO_10PT_H_
#define BAKEDFONTS_DOTO_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_10PT_WIDTH  = 6;
static const uint8_t DOTO_10PT_HEIGHT = 10;
static const uint32_t DOTO_10PT_FIRST  = 32;
static const uint32_t DOTO_10PT_LAST   = 8722;
static const uint16_t DOTO_10PT_COUNT  = 319;

extern const uint32_t doto_10pt_codes[];

extern const uint8_t doto_10pt_bits[] PROGMEM;
extern const uint8_t doto_10pt_widths[];
extern const FontHandle doto_10pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_10PT_H_ */
