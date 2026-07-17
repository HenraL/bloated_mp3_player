#ifndef BAKEDFONTS_WORKBENCH_14PT_H_
#define BAKEDFONTS_WORKBENCH_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_14PT_WIDTH  9
#define WORKBENCH_14PT_HEIGHT 15
#define WORKBENCH_14PT_FIRST  32
#define WORKBENCH_14PT_LAST   8725
#define WORKBENCH_14PT_COUNT  215

extern const uint16_t workbench_14pt_codes[];

extern const uint8_t workbench_14pt_bits[] PROGMEM;
extern const uint8_t workbench_14pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_14PT_H_ */
