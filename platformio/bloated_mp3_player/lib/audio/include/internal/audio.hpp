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
* FILE: audio.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* I2S audio output driver. Produces sound quality roughly equivalent to
* a dalek singing through a kazoo. This is intentional — it adds to the
* "character" of the device. Or so we tell ourselves.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: I2S-based audio playback (gloriously awful quality).
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>

namespace Audio
{
    struct WavHeader
    {
        char riff[4] = {'R','I','F','F'};
        uint32_t file_size = 0;
        char wave[4] = {'W','A','V','E'};
        char fmt_[4] = {'f','m','t',' '};
        uint32_t fmt_size = 16;
        uint16_t audio_fmt = 1;
        uint16_t channels = 1;
        uint32_t sample_rate = 22050;
        uint32_t byte_rate = 0;
        uint16_t block_align = 0;
        uint16_t bits_per_sample = 16;
        char data[4] = {'d','a','t','a'};
        uint32_t data_size = 0;
    };

    enum class State : uint8_t
    {
        Stopped,
        Playing,
        Paused,
        Error
    };

    bool begin(uint8_t bclk, uint8_t lrc, uint8_t dout);
    void set_volume(uint8_t vol);
    uint8_t get_volume();
    bool play(const char *wav_path);
    bool play_raw(const int16_t *samples, size_t count);
    void stop();
    void pause();
    void resume();
    State get_state();
    bool is_playing();
    void tick();
    uint32_t get_position_ms();
    uint32_t get_duration_ms();
}
