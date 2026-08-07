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
    { "The ships hung ",          My::Config::vogon_duration_scroll },
    { " ships hung in ",          My::Config::vogon_duration_scroll },
    { "ships hung in the ",       My::Config::vogon_duration_scroll },
    { "hips hung in the sk",      My::Config::vogon_duration_scroll },
    { "ps hung in the sky ",      My::Config::vogon_duration_scroll },
    { " hung in the sky in m",    My::Config::vogon_duration_scroll },
    { "ng in the sky in much",    My::Config::vogon_duration_scroll },
    { " in the sky in much the",  My::Config::vogon_duration_scroll },
    { "the sky in much the sam",  My::Config::vogon_duration_scroll },
    { "e sky in much the same ",  My::Config::vogon_duration_scroll },
    { "kay in much the same w",   My::Config::vogon_duration_scroll },
    { " in much the same way t",  My::Config::vogon_duration_scroll },
    { "much the same way that ",  My::Config::vogon_duration_scroll },
    { "h the same way that bro",  My::Config::vogon_duration_scroll },
    { "he same way that brick",   My::Config::vogon_duration_scroll },
    { "e same way that bricks ",  My::Config::vogon_duration_scroll },
    { "same way that bricks don", My::Config::vogon_duration_scroll },
    { "way that bricks don't.",   My::Config::vogon_duration_reveal }
};

// "Don't panic. Bring a towel."
static const My::Config::Structures::VogonStep banner_towel[] PROGMEM =
{
    { "Don't panic. ",       My::Config::vogon_duration_scroll },
    { "n't panic. Bring ",   My::Config::vogon_duration_scroll },
    { "t panic. Bring a ",   My::Config::vogon_duration_scroll },
    { " panic. Bring a tow", My::Config::vogon_duration_scroll },
    { "panic. Bring a towe", My::Config::vogon_duration_scroll },
    { "nic. Bring a towel.", My::Config::vogon_duration_reveal }
};

/* "In the beginning the Universe was created." */
static const My::Config::Structures::VogonStep banner_universe[] PROGMEM =
{
    { "In the beginning ",  My::Config::vogon_duration_scroll },
    { "the beginning the",  My::Config::vogon_duration_scroll },
    { " beginning the Uni", My::Config::vogon_duration_scroll },
    { "inning the Univers", My::Config::vogon_duration_scroll },
    { "ng the Universe wa", My::Config::vogon_duration_scroll },
    { "the Universe was cr", My::Config::vogon_duration_scroll },
    { "e Universe was crea", My::Config::vogon_duration_scroll },
    { "Universe was create", My::Config::vogon_duration_scroll },
    { "verse was created. ", My::Config::vogon_duration_reveal }
};

/* "42 — the ultimate answer." */
static const My::Config::Structures::VogonStep banner_42[] PROGMEM =
{
    { "42 - the answer. ", My::Config::vogon_duration_scroll },
    { "2 - the answer to ", My::Config::vogon_duration_scroll },
    { "the answer to eve",  My::Config::vogon_duration_scroll },
    { "e answer to everyt", My::Config::vogon_duration_scroll },
    { "answer to everythi", My::Config::vogon_duration_reveal }
};

// ─────────────────────────────────────────────────────────────────────────
// The banner sequence. Ends with a {nullptr, 0} sentinel.
const My::Config::Structures::VogonMessage My::Config::vogon_messages[] PROGMEM = {
    { banner_ships,     sizeof(banner_ships) / sizeof(banner_ships[0])     },
    { banner_towel,     sizeof(banner_towel) / sizeof(banner_towel[0])     },
    { banner_universe,  sizeof(banner_universe) / sizeof(banner_universe[0])  },
    { banner_42,        sizeof(banner_42) / sizeof(banner_42[0])        },

    { nullptr, 0 }
};

// End-of-banner blip: a quick underline sweep to clear the road for the
// next Vogon stanza. The tasteful version of a fond slap on the screen.
static const My::Config::Structures::VogonStep eom_1[] PROGMEM =
{
    { "----------------", My::Config::vogon_duration_eom },
    { "                ", My::Config::vogon_duration_eom }
};

const My::Config::Structures::VogonMessage My::Config::vogon_eom[] PROGMEM = {
    { eom_1, sizeof(eom_1) / sizeof(eom_1[0]) },

    { nullptr, 0 }
};
