#ifndef BAKEDFONTS_DATATYPE_10PT_H_
#define BAKEDFONTS_DATATYPE_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_10PT_WIDTH  7
#define DATATYPE_10PT_HEIGHT 11
#define DATATYPE_10PT_FIRST  32
#define DATATYPE_10PT_LAST   8722
#define DATATYPE_10PT_COUNT  319

extern const uint16_t datatype_10pt_codes[];

extern const uint8_t datatype_10pt_bits[] PROGMEM;
extern const uint8_t datatype_10pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_10PT_H_ */
