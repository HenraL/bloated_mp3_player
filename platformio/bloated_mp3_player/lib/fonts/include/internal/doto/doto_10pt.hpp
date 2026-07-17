#ifndef BAKEDFONTS_DOTO_10PT_H_
#define BAKEDFONTS_DOTO_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_10PT_WIDTH  6
#define DOTO_10PT_HEIGHT 10
#define DOTO_10PT_FIRST  32
#define DOTO_10PT_LAST   8722
#define DOTO_10PT_COUNT  319

extern const uint16_t doto_10pt_codes[];

extern const uint8_t doto_10pt_bits[] PROGMEM;
extern const uint8_t doto_10pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_10PT_H_ */
