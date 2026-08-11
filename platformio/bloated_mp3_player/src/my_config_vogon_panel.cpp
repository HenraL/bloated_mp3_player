/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* .......................
* ...><>.............<><.
* ..><>.><>.......<><.<><
* .><>.<><.><>.<><.<><.<>
* ..><>.><>.......<><.<><
* ...><>.............<><.
* .......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: my_config_vogon_panel.cpp
* CREATION DATE: 07-08-2026
* LAST Modified: 1:36:23 08-08-2026
* DESCRIPTION:
* The Vogon panel banner table. Each banner is a run of "frames": a
* scrolling text frame plus a dwell. The task walks this list exactly
* like the LED task walks its morse tables.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Vogon panel banner timeline data.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "my/config/vogon_panel.hpp"

// ─── Banners ────────────────────────────────────────────────────────────
// Each banner is three frames of the same quote, each frame scrolled
// one character further right. Reveals fill the rest of the line with
// spaces so old glyphs get erased.

// "The ships hung in the sky in much the same way that bricks don't."
static const My::Config::Structures::VogonStep banner_ships[] PROGMEM =
{
    { "The ships hung ",          0 },
    { " ships hung in ",          0 },
    { "ships hung in the ",       0 },
    { "hips hung in the sk",      0 },
    { "ps hung in the sky ",      0 },
    { " hung in the sky in m",    0 },
    { "ng in the sky in much",    0 },
    { " in the sky in much the",  0 },
    { "the sky in much the sam",  0 },
    { "e sky in much the same ",  0 },
    { "kay in much the same w",   0 },
    { " in much the same way t",  0 },
    { "much the same way that ",  0 },
    { "h the same way that bro",  0 },
    { "he same way that brick",   0 },
    { "e same way that bricks ",  0 },
    { "same way that bricks don", 0 },
    { "way that bricks don't.",   0 }
};

// "Don't panic. Bring a towel."
static const My::Config::Structures::VogonStep banner_towel[] PROGMEM =
{
    { "Don't panic. ",       0 },
    { "n't panic. Bring ",   0 },
    { "t panic. Bring a ",   0 },
    { " panic. Bring a tow", 0 },
    { "panic. Bring a towe", 0 },
    { "nic. Bring a towel.", 0 }
};

/* "In the beginning the Universe was created." */
static const My::Config::Structures::VogonStep banner_universe[] PROGMEM =
{
    { "In the beginning ",  0 },
    { "the beginning the",  0 },
    { " beginning the Uni", 0 },
    { "inning the Univers", 0 },
    { "ng the Universe wa", 0 },
    { "the Universe was cr", 0 },
    { "e Universe was crea", 0 },
    { "Universe was create", 0 },
    { "verse was created. ", 0 }
};

/* "42 — the ultimate answer." */
static const My::Config::Structures::VogonStep banner_42[] PROGMEM =
{
    { "42 - the answer. ", 0 },
    { "2 - the answer to ", 0 },
    { "the answer to eve",  0 },
    { "e answer to everyt", 0 },
    { "answer to everythi", 0 }
};

// ─────────────────────────────────────────────────────────────────────────
// The banner sequence. Ends with a {nullptr, 0, 0} sentinel.
// Each banner carries its own total duration (ms): the task paces the
// frames with duration / frame_count, so a longer quote takes exactly
// as long to read as the Vogon inside demands. Snail's pace, always.
const My::Config::Structures::VogonMessage My::Config::vogon_messages[] PROGMEM = {
    { banner_ships,    sizeof(banner_ships) / sizeof(banner_ships[0]),     16000 },
    { banner_towel,    sizeof(banner_towel) / sizeof(banner_towel[0]),      9000 },
    { banner_universe, sizeof(banner_universe) / sizeof(banner_universe[0]), 13500 },
    { banner_42,       sizeof(banner_42) / sizeof(banner_42[0]),           7500 },

    { nullptr, 0, 0 }
};

// End-of-banner blip: a quick underline sweep to clear the road for the
// next Vogon stanza. The tasteful version of a fond slap on the screen.
static const My::Config::Structures::VogonStep eom_1[] PROGMEM =
{
    { "----------------", My::Config::vogon_duration_eom },
    { "                ", My::Config::vogon_duration_eom }
};

const My::Config::Structures::VogonMessage My::Config::vogon_eom[] PROGMEM = {
    { eom_1, sizeof(eom_1) / sizeof(eom_1[0]), 0 },

    { nullptr, 0, 0 }
};
