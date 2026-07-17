#ifndef BAKEDFONTS_DATATYPE_8PT_H_
#define BAKEDFONTS_DATATYPE_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_8PT_WIDTH  6
#define DATATYPE_8PT_HEIGHT 9
#define DATATYPE_8PT_FIRST  32
#define DATATYPE_8PT_LAST   8722
#define DATATYPE_8PT_COUNT  319

extern const uint16_t datatype_8pt_codes[];

extern const uint8_t datatype_8pt_bits[] PROGMEM;
extern const uint8_t datatype_8pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_8PT_H_ */
