#ifndef BAKEDFONTS_DATATYPE_12PT_H_
#define BAKEDFONTS_DATATYPE_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_12PT_WIDTH  8
#define DATATYPE_12PT_HEIGHT 13
#define DATATYPE_12PT_FIRST  32
#define DATATYPE_12PT_LAST   8722
#define DATATYPE_12PT_COUNT  319

extern const uint16_t datatype_12pt_codes[];

extern const uint8_t datatype_12pt_bits[] PROGMEM;
extern const uint8_t datatype_12pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_12PT_H_ */
