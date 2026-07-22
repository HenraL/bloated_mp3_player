#ifndef BAKEDFONTS_BITCOUNTSINGLE_4PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_4PT_WIDTH  5
#define BITCOUNTSINGLE_4PT_HEIGHT 4
#define BITCOUNTSINGLE_4PT_FIRST  32
#define BITCOUNTSINGLE_4PT_LAST   64258
#define BITCOUNTSINGLE_4PT_COUNT  396

extern const uint16_t bitcountsingle_4pt_codes[];

extern const uint8_t bitcountsingle_4pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_4pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_4PT_H_ */
