#ifndef BAKEDFONTS_DATATYPE_2PT_H_
#define BAKEDFONTS_DATATYPE_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_2PT_WIDTH  2
#define DATATYPE_2PT_HEIGHT 3
#define DATATYPE_2PT_FIRST  32
#define DATATYPE_2PT_LAST   8722
#define DATATYPE_2PT_COUNT  319

extern const uint16_t datatype_2pt_codes[];

extern const uint8_t datatype_2pt_bits[] PROGMEM;
extern const uint8_t datatype_2pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_2PT_H_ */
