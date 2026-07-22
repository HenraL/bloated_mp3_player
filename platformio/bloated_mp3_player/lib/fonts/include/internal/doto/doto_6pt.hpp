#ifndef BAKEDFONTS_DOTO_6PT_H_
#define BAKEDFONTS_DOTO_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_6PT_WIDTH  4
#define DOTO_6PT_HEIGHT 7
#define DOTO_6PT_FIRST  32
#define DOTO_6PT_LAST   8722
#define DOTO_6PT_COUNT  319

extern const uint16_t doto_6pt_codes[];

extern const uint8_t doto_6pt_bits[] PROGMEM;
extern const uint8_t doto_6pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_6PT_H_ */
