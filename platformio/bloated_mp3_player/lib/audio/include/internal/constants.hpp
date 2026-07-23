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
* CREATION DATE: 23-07-2026
* DESCRIPTION:
* Magic numbers, limits, and pin defaults for the Audio library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Centralise all audio-related magic values.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>

namespace Audio
{

    static const uint32_t PWM_FREQ_HZ = 200000;
    static const uint32_t STOP_DUTY = 128;

    static const uint32_t US_PER_SEC = 1000000;
    static const uint32_t DEFAULT_US_PER_FRAME = 23;
    static const uint32_t VOLUME_SHIFT = 8;
    static const uint32_t PCM_OFFSET = 32768;
    static const uint32_t PCM_SHIFT = 8;
    static const uint32_t DEFAULT_SAMPLE_RATE = 44100;

    static const uint32_t WAV_CHUNK_ID_LEN = 4;
    static const uint32_t WAV_CHUNK_HEADER_LEN = 8;
    static const uint32_t WAV_CHUNK_ALIGN = 1;

    static const uint32_t MP3_SYNC_MIN = 4;
    static const uint32_t MP3_IN_BUF_SIZE = 4096;
    static const uint32_t MP3_FRAME_PCM_SIZE = 2304;

    static const uint32_t PLAYER_MAX_FRAMES  = 512;

}
