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
* FILE: sdcard_structures.hpp
* CREATION DATE: 23-07-2026
* LAST Modified: 12:12:0 23-07-2026
* DESCRIPTION:
* Data structures for the SDCard library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: TrackInfo, DirEntry, and related types.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
#include "sdcard_constants.hpp"

namespace SDCard
{
    struct TrackInfo
    {
        char path[TRACK_PATH_LEN];
        char filename[TRACK_FILENAME_LEN];
        uint32_t size;
        bool is_wav;
    };

    struct DirEntry
    {
        char name[DIR_ENTRY_NAME_LEN];
        bool is_dir;
    };
}
