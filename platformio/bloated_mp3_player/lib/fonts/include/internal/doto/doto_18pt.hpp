#ifndef BAKEDFONTS_DOTO_18PT_H_
#define BAKEDFONTS_DOTO_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_18PT_WIDTH  = 11;
static const uint8_t DOTO_18PT_HEIGHT = 34;
static const uint32_t DOTO_18PT_FIRST  = 32;
static const uint32_t DOTO_18PT_LAST   = 8722;
static const uint16_t DOTO_18PT_COUNT  = 319;

extern const uint32_t doto_18pt_codes[];

extern const uint8_t doto_18pt_bits[] PROGMEM;
extern const uint8_t doto_18pt_widths[];
extern const FontHandle doto_18pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_18PT_H_ */
