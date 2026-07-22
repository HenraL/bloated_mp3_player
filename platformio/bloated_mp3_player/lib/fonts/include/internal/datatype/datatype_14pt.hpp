#ifndef BAKEDFONTS_DATATYPE_14PT_H_
#define BAKEDFONTS_DATATYPE_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_14PT_WIDTH  = 9;
static const uint8_t DATATYPE_14PT_HEIGHT = 32;
static const uint8_t DATATYPE_14PT_ASCENT = 14;
static const uint32_t DATATYPE_14PT_FIRST  = 32;
static const uint32_t DATATYPE_14PT_LAST   = 8722;
static const uint16_t DATATYPE_14PT_COUNT  = 319;

extern const uint32_t datatype_14pt_codes[];

extern const uint8_t datatype_14pt_bits[] PROGMEM;
extern const uint8_t datatype_14pt_widths[];
extern const FontHandle datatype_14pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_14PT_H_ */
