#ifndef BAKEDFONTS_DOTO_12PT_H_
#define BAKEDFONTS_DOTO_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_12PT_WIDTH  8
#define DOTO_12PT_HEIGHT 13
#define DOTO_12PT_FIRST  32
#define DOTO_12PT_LAST   8722
#define DOTO_12PT_COUNT  319

extern const uint16_t doto_12pt_codes[];

extern const uint8_t doto_12pt_bits[] PROGMEM;
extern const uint8_t doto_12pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_12PT_H_ */
