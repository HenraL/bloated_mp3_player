#ifndef BAKEDFONTS_JACQUARD12_14PT_H_
#define BAKEDFONTS_JACQUARD12_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define JACQUARD12_14PT_WIDTH  20
#define JACQUARD12_14PT_HEIGHT 14
#define JACQUARD12_14PT_FIRST  32
#define JACQUARD12_14PT_LAST   8722
#define JACQUARD12_14PT_COUNT  331

extern const uint16_t jacquard12_14pt_codes[];

extern const uint8_t jacquard12_14pt_regular_bits[] PROGMEM;
extern const uint8_t jacquard12_14pt_regular_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_JACQUARD12_14PT_H_ */
