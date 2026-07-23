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
* FILE: sdcard_constants.hpp
* CREATION DATE: 23-07-2026
* LAST Modified: 12:10:0 23-07-2026
* DESCRIPTION:
* Centralised magic values for the SDCard library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Buffer sizes, limits, and magic constants.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stddef.h>

namespace SDCard
{
    static const size_t MAX_TRACKS = 256;
    static const size_t MAX_DIR_ENTRIES = 128;
    static const size_t TRACK_PATH_LEN = 64;
    static const size_t TRACK_FILENAME_LEN = 32;
    static const size_t DIR_ENTRY_NAME_LEN = 32;
    static const size_t MAX_SUBPATH_LEN = 128;
    static const size_t NUM_AUDIO_EXT = 4;
}
