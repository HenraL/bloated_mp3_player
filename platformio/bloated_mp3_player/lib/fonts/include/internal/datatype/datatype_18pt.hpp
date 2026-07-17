#ifndef BAKEDFONTS_DATATYPE_18PT_H_
#define BAKEDFONTS_DATATYPE_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_18PT_WIDTH  12
#define DATATYPE_18PT_HEIGHT 19
#define DATATYPE_18PT_FIRST  32
#define DATATYPE_18PT_LAST   8722
#define DATATYPE_18PT_COUNT  319

extern const uint16_t datatype_18pt_codes[];

extern const uint8_t datatype_18pt_bits[] PROGMEM;
extern const uint8_t datatype_18pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_18PT_H_ */
