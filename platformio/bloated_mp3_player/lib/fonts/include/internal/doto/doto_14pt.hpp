#ifndef BAKEDFONTS_DOTO_14PT_H_
#define BAKEDFONTS_DOTO_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_14PT_WIDTH  9
#define DOTO_14PT_HEIGHT 15
#define DOTO_14PT_FIRST  32
#define DOTO_14PT_LAST   8722
#define DOTO_14PT_COUNT  319

extern const uint16_t doto_14pt_codes[];

extern const uint8_t doto_14pt_bits[] PROGMEM;
extern const uint8_t doto_14pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_14PT_H_ */
