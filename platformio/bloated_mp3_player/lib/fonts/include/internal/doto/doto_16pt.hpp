#ifndef BAKEDFONTS_DOTO_16PT_H_
#define BAKEDFONTS_DOTO_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_16PT_WIDTH  10
#define DOTO_16PT_HEIGHT 17
#define DOTO_16PT_FIRST  32
#define DOTO_16PT_LAST   8722
#define DOTO_16PT_COUNT  319

extern const uint16_t doto_16pt_codes[];

extern const uint8_t doto_16pt_bits[] PROGMEM;
extern const uint8_t doto_16pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_16PT_H_ */
