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
* FILE: wav_decoder.hpp
* CREATION DATE: 23-07-2026
* DESCRIPTION:
* WAV file decoder. Handles RIFF/WAVE PCM 16-bit mono/stereo.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Decoder implementation for .wav files.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include "decoder.hpp"
#include "constants.hpp"
#include <Arduino.h>
#include <SD_MMC.h>
#include "structs.hpp"

namespace Audio
{

    class WavDecoder : public Decoder
    {
    public:
        WavDecoder() {}
        ~WavDecoder() override { close(); }

        bool     open(const char *path) override;
        void     close() override;

        int      read(int16_t *pcm, size_t max_frames) override;

        uint32_t sample_rate() const override { return _sr; }
        uint16_t channels() const override { return _channels; }
        bool     eof() const override { return _eof; }
        bool     is_open() const override { return _file ? true : false; }

    private:
        File     _file;
        uint32_t _sr = 0;
        uint16_t _channels = 0;
        uint32_t _data_start = 0;
        uint32_t _data_left = 0;
        bool     _eof = false;
    };

}
