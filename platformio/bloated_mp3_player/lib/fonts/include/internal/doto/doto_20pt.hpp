#ifndef BAKEDFONTS_DOTO_20PT_H_
#define BAKEDFONTS_DOTO_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_20PT_WIDTH  12
#define DOTO_20PT_HEIGHT 20
#define DOTO_20PT_FIRST  32
#define DOTO_20PT_LAST   8722
#define DOTO_20PT_COUNT  319

extern const uint16_t doto_20pt_codes[];

extern const uint8_t doto_20pt_bits[] PROGMEM;
extern const uint8_t doto_20pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_20PT_H_ */
