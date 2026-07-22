#ifndef BAKEDFONTS_DATATYPE_16PT_H_
#define BAKEDFONTS_DATATYPE_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_16PT_WIDTH  = 11;
static const uint8_t DATATYPE_16PT_HEIGHT = 33;
static const uint32_t DATATYPE_16PT_FIRST  = 32;
static const uint32_t DATATYPE_16PT_LAST   = 8722;
static const uint16_t DATATYPE_16PT_COUNT  = 319;

extern const uint32_t datatype_16pt_codes[];

extern const uint8_t datatype_16pt_bits[] PROGMEM;
extern const uint8_t datatype_16pt_widths[];
extern const FontHandle datatype_16pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_16PT_H_ */
