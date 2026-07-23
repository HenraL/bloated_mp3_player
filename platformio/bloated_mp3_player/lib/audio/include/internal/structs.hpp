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
* FILE: structs.hpp
* CREATION DATE: 23-07-2026
* DESCRIPTION:
* POD structures and data types for the Audio library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: WavHeader and other audio-related structs.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>

namespace Audio
{
    struct WavHeader
    {
        char     riff[4]       = {'R','I','F','F'};
        uint32_t file_size     = 0;
        char     wave[4]       = {'W','A','V','E'};
        char     fmt_[4]       = {'f','m','t',' '};
        uint32_t fmt_size      = 16;
        uint16_t audio_fmt     = 1;
        uint16_t channels      = 1;
        uint32_t sample_rate   = 22050;
        uint32_t byte_rate     = 0;
        uint16_t block_align   = 0;
        uint16_t bits_per_sample = 16;
        char     data[4]       = {'d','a','t','a'};
        uint32_t data_size     = 0;
    };
}
