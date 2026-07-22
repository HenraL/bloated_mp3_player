#ifndef BAKEDFONTS_WORKBENCH_10PT_H_
#define BAKEDFONTS_WORKBENCH_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_10PT_WIDTH  = 7;
static const uint8_t WORKBENCH_10PT_HEIGHT = 11;
static const uint32_t WORKBENCH_10PT_FIRST  = 32;
static const uint32_t WORKBENCH_10PT_LAST   = 8725;
static const uint16_t WORKBENCH_10PT_COUNT  = 215;

extern const uint32_t workbench_10pt_codes[];

extern const uint8_t workbench_10pt_bits[] PROGMEM;
extern const uint8_t workbench_10pt_widths[];
extern const FontHandle workbench_10pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_10PT_H_ */
