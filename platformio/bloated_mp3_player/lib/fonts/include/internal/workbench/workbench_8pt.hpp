#ifndef BAKEDFONTS_WORKBENCH_8PT_H_
#define BAKEDFONTS_WORKBENCH_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_8PT_WIDTH  = 6;
static const uint8_t WORKBENCH_8PT_HEIGHT = 15;
static const uint32_t WORKBENCH_8PT_FIRST  = 32;
static const uint32_t WORKBENCH_8PT_LAST   = 8725;
static const uint16_t WORKBENCH_8PT_COUNT  = 215;

extern const uint32_t workbench_8pt_codes[];

extern const uint8_t workbench_8pt_bits[] PROGMEM;
extern const uint8_t workbench_8pt_widths[];
extern const FontHandle workbench_8pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_8PT_H_ */
