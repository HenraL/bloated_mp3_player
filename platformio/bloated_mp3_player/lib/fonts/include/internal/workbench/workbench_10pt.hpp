#ifndef BAKEDFONTS_WORKBENCH_10PT_H_
#define BAKEDFONTS_WORKBENCH_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_10PT_WIDTH  7
#define WORKBENCH_10PT_HEIGHT 11
#define WORKBENCH_10PT_FIRST  32
#define WORKBENCH_10PT_LAST   8725
#define WORKBENCH_10PT_COUNT  215

extern const uint16_t workbench_10pt_codes[];

extern const uint8_t workbench_10pt_bits[] PROGMEM;
extern const uint8_t workbench_10pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_10PT_H_ */
