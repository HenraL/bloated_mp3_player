#ifndef BAKEDFONTS_DATATYPE_14PT_H_
#define BAKEDFONTS_DATATYPE_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_14PT_WIDTH  10
#define DATATYPE_14PT_HEIGHT 15
#define DATATYPE_14PT_FIRST  32
#define DATATYPE_14PT_LAST   8722
#define DATATYPE_14PT_COUNT  319

extern const uint16_t datatype_14pt_codes[];

extern const uint8_t datatype_14pt_bits[] PROGMEM;
extern const uint8_t datatype_14pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_14PT_H_ */
