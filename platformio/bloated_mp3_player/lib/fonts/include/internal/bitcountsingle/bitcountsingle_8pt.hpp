#ifndef BAKEDFONTS_BITCOUNTSINGLE_8PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_8PT_WIDTH  10
#define BITCOUNTSINGLE_8PT_HEIGHT 8
#define BITCOUNTSINGLE_8PT_FIRST  32
#define BITCOUNTSINGLE_8PT_LAST   64258
#define BITCOUNTSINGLE_8PT_COUNT  396

extern const uint16_t bitcountsingle_8pt_codes[];

extern const uint8_t bitcountsingle_8pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_8pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_8PT_H_ */
