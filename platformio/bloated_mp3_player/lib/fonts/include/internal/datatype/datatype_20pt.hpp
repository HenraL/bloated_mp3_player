#ifndef BAKEDFONTS_DATATYPE_20PT_H_
#define BAKEDFONTS_DATATYPE_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_20PT_WIDTH  13
#define DATATYPE_20PT_HEIGHT 21
#define DATATYPE_20PT_FIRST  32
#define DATATYPE_20PT_LAST   8722
#define DATATYPE_20PT_COUNT  319

extern const uint16_t datatype_20pt_codes[];

extern const uint8_t datatype_20pt_bits[] PROGMEM;
extern const uint8_t datatype_20pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_20PT_H_ */
