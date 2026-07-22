#ifndef BAKEDFONTS_DATATYPE_18PT_H_
#define BAKEDFONTS_DATATYPE_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_18PT_WIDTH  = 12;
static const uint8_t DATATYPE_18PT_HEIGHT = 38;
static const uint32_t DATATYPE_18PT_FIRST  = 32;
static const uint32_t DATATYPE_18PT_LAST   = 8722;
static const uint16_t DATATYPE_18PT_COUNT  = 319;

extern const uint32_t datatype_18pt_codes[];

extern const uint8_t datatype_18pt_bits[] PROGMEM;
extern const uint8_t datatype_18pt_widths[];
extern const FontHandle datatype_18pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_18PT_H_ */
