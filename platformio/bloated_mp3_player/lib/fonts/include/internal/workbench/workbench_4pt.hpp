#ifndef BAKEDFONTS_WORKBENCH_4PT_H_
#define BAKEDFONTS_WORKBENCH_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_4PT_WIDTH  4
#define WORKBENCH_4PT_HEIGHT 4
#define WORKBENCH_4PT_FIRST  32
#define WORKBENCH_4PT_LAST   8725
#define WORKBENCH_4PT_COUNT  215

extern const uint16_t workbench_4pt_codes[];

extern const uint8_t workbench_4pt_bits[] PROGMEM;
extern const uint8_t workbench_4pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_4PT_H_ */
