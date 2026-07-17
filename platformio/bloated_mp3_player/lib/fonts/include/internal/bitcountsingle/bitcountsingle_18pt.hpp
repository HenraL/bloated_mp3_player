#ifndef BAKEDFONTS_BITCOUNTSINGLE_18PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_18PT_WIDTH  22
#define BITCOUNTSINGLE_18PT_HEIGHT 17
#define BITCOUNTSINGLE_18PT_FIRST  32
#define BITCOUNTSINGLE_18PT_LAST   64258
#define BITCOUNTSINGLE_18PT_COUNT  396

extern const uint16_t bitcountsingle_18pt_codes[];

extern const uint8_t bitcountsingle_18pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_18pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_18PT_H_ */
