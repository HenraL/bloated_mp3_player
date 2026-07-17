#ifndef BAKEDFONTS_WORKBENCH_18PT_H_
#define BAKEDFONTS_WORKBENCH_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_18PT_WIDTH  11
#define WORKBENCH_18PT_HEIGHT 18
#define WORKBENCH_18PT_FIRST  32
#define WORKBENCH_18PT_LAST   8725
#define WORKBENCH_18PT_COUNT  215

extern const uint16_t workbench_18pt_codes[];

extern const uint8_t workbench_18pt_bits[] PROGMEM;
extern const uint8_t workbench_18pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_18PT_H_ */
