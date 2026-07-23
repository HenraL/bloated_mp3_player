#pragma once
#include <Arduino.h>
#include <SD_MMC.h>
#include "audio.hpp"

namespace Audio
{

    class Player
    {
    public:
        Player(Audio &audio);

        bool        load(const char *path);
        void        unload();
        void        tick();

        bool        is_loaded() const;

    private:
        Audio      &_audio;
        File        _file;
        uint32_t    _data_start = 0;
        uint32_t    _data_left  = 0;
        volatile bool _loading = false;
    };

} // namespace Audio
