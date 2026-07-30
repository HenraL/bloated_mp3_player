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
* FILE: constants.hpp
* CREATION DATE: 30-07-2026
* LAST Modified: 30-07-2026
* DESCRIPTION:
* Compile-time limits for the Profiler library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Profiler tuning constants.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>

namespace ProfilerConstants
{
    static const uint16_t MAX_DEPTH = 16;
    static const uint16_t MAX_TRACKED_TASKS = 8;
    static const uint16_t MAX_PATH_LEN = 384;
}
