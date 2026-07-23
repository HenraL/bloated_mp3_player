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
* FILE: decoder.hpp
* CREATION DATE: 23-07-2026
* DESCRIPTION:
* Abstract decoder interface — swap WavDecoder, Mp3Decoder,
* or any other decoder without touching Player code.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Polymorphic audio decoder base class.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Audio
{

    class Decoder
    {
    public:
        virtual ~Decoder() {}

        virtual bool     open(const char *path) = 0;
        virtual void     close() = 0;

        virtual int      read(int16_t *pcm, size_t max_frames) = 0;

        virtual uint32_t sample_rate() const = 0;
        virtual uint16_t channels() const = 0;
        virtual bool     eof() const = 0;
        virtual bool     is_open() const = 0;
    };

}
