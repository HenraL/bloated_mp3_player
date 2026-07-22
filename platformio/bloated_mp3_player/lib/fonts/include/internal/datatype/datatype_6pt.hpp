#ifndef BAKEDFONTS_DATATYPE_6PT_H_
#define BAKEDFONTS_DATATYPE_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_6PT_WIDTH  4
#define DATATYPE_6PT_HEIGHT 7
#define DATATYPE_6PT_FIRST  32
#define DATATYPE_6PT_LAST   8722
#define DATATYPE_6PT_COUNT  319

extern const uint16_t datatype_6pt_codes[];

extern const uint8_t datatype_6pt_bits[] PROGMEM;
extern const uint8_t datatype_6pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_6PT_H_ */
