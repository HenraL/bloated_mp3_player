#ifndef BAKEDFONTS_DOTO_18PT_H_
#define BAKEDFONTS_DOTO_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_18PT_WIDTH  11
#define DOTO_18PT_HEIGHT 19
#define DOTO_18PT_FIRST  32
#define DOTO_18PT_LAST   8722
#define DOTO_18PT_COUNT  319

extern const uint16_t doto_18pt_codes[];

extern const uint8_t doto_18pt_bits[] PROGMEM;
extern const uint8_t doto_18pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_18PT_H_ */
