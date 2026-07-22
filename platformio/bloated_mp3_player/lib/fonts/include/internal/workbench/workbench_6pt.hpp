#ifndef BAKEDFONTS_WORKBENCH_6PT_H_
#define BAKEDFONTS_WORKBENCH_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_6PT_WIDTH  5
#define WORKBENCH_6PT_HEIGHT 7
#define WORKBENCH_6PT_FIRST  32
#define WORKBENCH_6PT_LAST   8725
#define WORKBENCH_6PT_COUNT  215

extern const uint16_t workbench_6pt_codes[];

extern const uint8_t workbench_6pt_bits[] PROGMEM;
extern const uint8_t workbench_6pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_6PT_H_ */
