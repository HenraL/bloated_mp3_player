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
* FILE: mp3_decoder.hpp
* CREATION DATE: 23-07-2026
* DESCRIPTION:
* MP3 decoder using the Helix fixed-point decoder.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Decoder implementation for .mp3 files.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include "decoder.hpp"
#include "constants.hpp"
#include <Arduino.h>
#include <SD_MMC.h>

namespace Audio
{

    class Mp3Decoder : public Decoder
    {
    public:
        Mp3Decoder();
        ~Mp3Decoder() override;

        bool     open(const char *path) override;
        void     close() override;

        int      read(int16_t *pcm, size_t max_frames) override;

        uint32_t sample_rate() const override { return _sr; }
        uint16_t channels() const override { return _channels; }
        bool     eof() const override { return _eof; }
        bool     is_open() const override { return _file ? true : false; }
        const char* diag() const override { return _diag_str; }

    private:
        void         *_decoder;
        File          _file;
        uint32_t      _sr;
        uint16_t      _channels;
        bool          _eof;

        uint8_t       _in_buf[MP3_IN_BUF_SIZE];
        int           _in_avail;
        int           _in_pos;
        int16_t       _frame_pcm[MP3_FRAME_PCM_SIZE];
        int           _frame_samples;
        int           _frame_consumed;
        char          _diag_str[256];

        void refill();
        void _diag_printf(const char *fmt, ...);
    };

}
