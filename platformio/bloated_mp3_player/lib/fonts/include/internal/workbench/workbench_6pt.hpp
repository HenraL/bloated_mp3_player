#ifndef BAKEDFONTS_WORKBENCH_6PT_H_
#define BAKEDFONTS_WORKBENCH_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_6PT_WIDTH  = 5;
static const uint8_t WORKBENCH_6PT_HEIGHT = 7;
static const uint32_t WORKBENCH_6PT_FIRST  = 32;
static const uint32_t WORKBENCH_6PT_LAST   = 8725;
static const uint16_t WORKBENCH_6PT_COUNT  = 215;

extern const uint32_t workbench_6pt_codes[];

extern const uint8_t workbench_6pt_bits[] PROGMEM;
extern const uint8_t workbench_6pt_widths[];
extern const FontHandle workbench_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_6PT_H_ */
