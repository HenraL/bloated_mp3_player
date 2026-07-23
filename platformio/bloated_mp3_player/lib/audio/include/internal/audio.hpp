#pragma once
#include <Arduino.h>
#include <driver/ledc.h>
#include "constants.hpp"
#include "structs.hpp"


namespace Audio
{
    enum Status
    {
        Stopped,
        Playing,
        Paused
    };

    class Audio
    {
    public:
        Audio(uint8_t speaker_pin_1, uint8_t speaker_pin_2,
              uint8_t dma_buf_count, uint16_t dma_buf_len);

        bool        open();

        void        play();
        void        pause();
        void        stop();

        void        setSampleRate(uint32_t sr);
        size_t      write(const int16_t *samples, size_t count);

        void        setVolume(uint8_t vol);
        uint8_t     getVolume() const;
        Status      getStatus() const;

    private:
        uint8_t     _speaker_pin_1;
        uint8_t     _speaker_pin_2;
        uint8_t     _dma_buf_count;
        uint16_t    _dma_buf_len;
        Status      _status    = Stopped;
        uint8_t     _volume    = 128;
        uint32_t    _sr        = 44100;
        ledc_channel_t _ledc_chan_1;
        ledc_channel_t _ledc_chan_2;
    };

} // namespace Audio
