#ifndef BAKEDFONTS_DATATYPE_10PT_H_
#define BAKEDFONTS_DATATYPE_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_10PT_WIDTH  = 7;
static const uint8_t DATATYPE_10PT_HEIGHT = 21;
static const uint32_t DATATYPE_10PT_FIRST  = 32;
static const uint32_t DATATYPE_10PT_LAST   = 8722;
static const uint16_t DATATYPE_10PT_COUNT  = 319;

extern const uint32_t datatype_10pt_codes[];

extern const uint8_t datatype_10pt_bits[] PROGMEM;
extern const uint8_t datatype_10pt_widths[];
extern const FontHandle datatype_10pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_10PT_H_ */
