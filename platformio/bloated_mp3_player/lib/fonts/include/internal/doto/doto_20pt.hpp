#ifndef BAKEDFONTS_DOTO_20PT_H_
#define BAKEDFONTS_DOTO_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_20PT_WIDTH  = 12;
static const uint8_t DOTO_20PT_HEIGHT = 20;
static const uint32_t DOTO_20PT_FIRST  = 32;
static const uint32_t DOTO_20PT_LAST   = 8722;
static const uint16_t DOTO_20PT_COUNT  = 319;

extern const uint32_t doto_20pt_codes[];

extern const uint8_t doto_20pt_bits[] PROGMEM;
extern const uint8_t doto_20pt_widths[];
extern const FontHandle doto_20pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_20PT_H_ */
