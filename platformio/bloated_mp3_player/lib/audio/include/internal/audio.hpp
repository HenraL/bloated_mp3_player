#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include "struct.hpp"


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
        Audio(uint8_t bclk, uint8_t lrc, uint8_t dout, uint8_t dout2,
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
        uint8_t     _bclk;
        uint8_t     _lrc;
        uint8_t     _dout;
        uint8_t     _dout2;
        uint8_t     _dma_buf_count;
        uint16_t    _dma_buf_len;
        Status      _status    = Stopped;
        uint8_t     _volume    = 128;
        uint32_t    _sr        = 44100;
        i2s_port_t  _i2s_port  = I2S_NUM_0;
    };

} // namespace Audio

#include "player.hpp"
