#ifndef BAKEDFONTS_BITCOUNTSINGLE_14PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_14PT_WIDTH  17
#define BITCOUNTSINGLE_14PT_HEIGHT 13
#define BITCOUNTSINGLE_14PT_FIRST  32
#define BITCOUNTSINGLE_14PT_LAST   64258
#define BITCOUNTSINGLE_14PT_COUNT  396

extern const uint16_t bitcountsingle_14pt_codes[];

extern const uint8_t bitcountsingle_14pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_14pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_14PT_H_ */
