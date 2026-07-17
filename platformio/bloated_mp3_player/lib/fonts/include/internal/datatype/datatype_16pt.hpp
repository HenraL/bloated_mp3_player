#ifndef BAKEDFONTS_DATATYPE_16PT_H_
#define BAKEDFONTS_DATATYPE_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_16PT_WIDTH  11
#define DATATYPE_16PT_HEIGHT 17
#define DATATYPE_16PT_FIRST  32
#define DATATYPE_16PT_LAST   8722
#define DATATYPE_16PT_COUNT  319

extern const uint16_t datatype_16pt_codes[];

extern const uint8_t datatype_16pt_bits[] PROGMEM;
extern const uint8_t datatype_16pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_16PT_H_ */
