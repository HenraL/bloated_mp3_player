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
* FILE: player.hpp
* CREATION DATE: 23-07-2026
* DESCRIPTION:
* Audio player that picks the right decoder (WAV / MP3) for each file.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: File-agnostic audio playback.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include "decoder.hpp"
#include "audio.hpp"

namespace Audio
{

    class Player
    {
    public:
        Player(Audio &audio);
        ~Player();

        bool load(const char *path);
        void unload();
        int  tick();

        bool is_loaded() const;
        bool track_finished() const;
        const char* last_diag() const;
        const char* track_name() const;

    private:
        Audio      &_audio;
        Decoder    *_decoder;
        volatile bool _loading;
        bool        _finished;
        int16_t     _tick_buf[PLAYER_MAX_FRAMES * 2];
        char        _last_diag[256];
        char        _track_name[128];
    };

}
