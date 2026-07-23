#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include "constants.hpp"
#include "structs.hpp"

namespace Audio
{
    static const size_t RING_CAPACITY = 65536;

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
        ~Audio();

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
        uint8_t      _speaker_pin;
        uint8_t      _mirror_pin; // unused, kept for API compat
        uint8_t      _dma_buf_count;
        uint16_t     _dma_buf_len;
        Status       _status    = Stopped;
        uint8_t      _volume    = 128;
        uint32_t     _sr        = 44100;
        i2s_port_t   _i2s_port;
        bool         _i2s_installed = false;

        int16_t     *_ring;
        size_t       _ring_mask;
        volatile size_t _ring_wp;
        volatile size_t _ring_rp;
        TaskHandle_t _output_task;

        size_t       _ring_avail() const;
        size_t       _ring_space() const;
        void         _ring_write(const int16_t *data, size_t count);
        size_t       _ring_read(int16_t *buf, size_t count);

        static void  _output_task_entry(void *arg);
        void         _output_task_loop();
    };

} // namespace Audio
