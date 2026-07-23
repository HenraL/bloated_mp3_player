#include "internal/mp3_decoder.hpp"
#include <sdcard.hpp>
#include <cstring>
#include <cstdio>

extern "C" {
#include "libhelix-mp3/mp3dec.h"
}

namespace Audio
{

    Mp3Decoder::Mp3Decoder()
        : _decoder(nullptr)
        , _sr(44100)
        , _channels(2)
        , _eof(false)
        , _in_avail(0)
        , _in_pos(0)
        , _pcm_ring(nullptr)
        , _ring_cap(0)
        , _ring_frames(0)
        , _ring_pos(0)
    {
        _diag_str[0] = '\0';
        _ring_alloc();
    }

    Mp3Decoder::~Mp3Decoder()
    {
        close();
        if (_pcm_ring)
        {
            free(_pcm_ring);
            _pcm_ring = nullptr;
        }
    }

    bool Mp3Decoder::_ring_alloc()
    {
        if (_pcm_ring)
        {
            return true;
        }
        size_t bytes = PCM_RING_FRAMES * 2 * sizeof(int16_t);
        _pcm_ring = (int16_t*)heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM);
        if (!_pcm_ring)
        {
            _pcm_ring = (int16_t*)malloc(bytes);
        }
        if (_pcm_ring)
        {
            _ring_cap = PCM_RING_FRAMES;
            _ring_frames = 0;
            _ring_pos = 0;
            return true;
        }
        _ring_cap = 0;
        return false;
    }

    void Mp3Decoder::_diag_printf(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(_diag_str, sizeof(_diag_str), fmt, args);
        va_end(args);
    }

    bool Mp3Decoder::open(const char *path)
    {
        close();

        _decoder = MP3InitDecoder();
        if (!_decoder)
        {
            return false;
        }

        File f = SDCard::open(path);
        if (!f)
        {
            MP3FreeDecoder(_decoder);
            _decoder = nullptr;
            return false;
        }

        _file = f;
        _eof = false;
        _in_avail = 0;
        _in_pos = 0;
        _ring_frames = 0;
        _ring_pos = 0;
        _diag_str[0] = '\0';
        _ring_alloc();

        return _pcm_ring != nullptr;
    }

    void Mp3Decoder::close()
    {
        if (_file)
        {
            _file.close();
        }
        if (_decoder)
        {
            MP3FreeDecoder(_decoder);
            _decoder = nullptr;
        }
        _sr = 44100;
        _channels = 2;
        _eof = false;
        _in_avail = 0;
        _in_pos = 0;
        _ring_frames = 0;
        _ring_pos = 0;
        _diag_str[0] = '\0';
    }

    void Mp3Decoder::refill()
    {
        if (_in_avail > 0 && _in_pos > 0)
        {
            size_t remaining = _in_avail;
            std::memmove(_in_buf, _in_buf + _in_pos, remaining);
            _in_avail = remaining;
            _in_pos = 0;
        }
        else if (_in_avail == 0)
        {
            _in_pos = 0;
        }

        int space = sizeof(_in_buf) - _in_avail;
        if (space > 0)
        {
            int n = _file.read(_in_buf + _in_avail, space);
            if (n > 0)
            {
                _in_avail += n;
            }
            if (n <= 0)
            {
                _eof = true;
            }
        }
    }

    bool Mp3Decoder::_decode_one_frame()
    {
        int tries = 0;
        while (tries < 4096)
        {
            tries++;
            refill();

            if (_in_avail <= 0)
            {
                _eof = true;
                break;
            }

            int sync = MP3FindSyncWord(_in_buf + _in_pos, _in_avail);
            if (sync < 0)
            {
                _in_avail = 0;
                continue;
            }

            _in_pos += sync;
            _in_avail -= sync;

            unsigned char *mp3_ptr = _in_buf + _in_pos;
            int mp3_left = _in_avail;

            if (mp3_left < (int)MP3_SYNC_MIN)
            {
                continue;
            }

            int result = MP3Decode(_decoder, &mp3_ptr, &mp3_left, _frame_pcm, 0);

            int consumed = _in_avail - mp3_left;
            if (consumed < 0)
            {
                consumed = 0;
            }
            _in_pos += consumed;
            _in_avail -= consumed;

            if (result == 0)
            {
                MP3FrameInfo info;
                MP3GetLastFrameInfo(_decoder, &info);

                _sr = info.samprate;
                _channels = info.nChans;
                int frame_samps = info.outputSamps;
                int frame_frames = frame_samps / info.nChans;

                size_t wp_samp = ((_ring_pos + _ring_frames) % _ring_cap) * 2;
                size_t ring_samps = _ring_cap * 2;
                size_t first = (size_t)frame_samps;
                if (first > ring_samps - wp_samp)
                {
                    first = ring_samps - wp_samp;
                }
                std::memcpy(_pcm_ring + wp_samp, _frame_pcm, first * sizeof(int16_t));
                if (first < (size_t)frame_samps)
                {
                    std::memcpy(_pcm_ring, _frame_pcm + first,
                               ((size_t)frame_samps - first) * sizeof(int16_t));
                }
                _ring_frames += frame_frames;
                return true;
            }

            if (consumed == 0)
            {
                _in_pos += 1;
                _in_avail -= 1;
            }
        }

        return false;
    }

    int Mp3Decoder::read(int16_t *pcm, size_t max_frames)
    {
        if (!_file || (_eof && _ring_frames == 0))
        {
            _diag_printf("eof");
            return 0;
        }

        // Refill: keep ring ≥ 75% full, fill to 100% when we do refill
        size_t refill_threshold = (_ring_cap * 3) / 4;
        if (_ring_frames < refill_threshold && !_eof)
        {
            do
            {
                if (!_decode_one_frame())
                {
                    break;
                }
                vTaskDelay(1);
            } while (_ring_frames < _ring_cap && !_eof);
        }

        size_t to_copy = (max_frames < _ring_frames) ? max_frames : _ring_frames;
        if (to_copy == 0)
        {
            _diag_printf("ring empty eof=%d", _eof);
            return 0;
        }

        size_t first = _ring_cap - _ring_pos;
        if (first > to_copy)
        {
            first = to_copy;
        }
        std::memcpy(pcm, _pcm_ring + _ring_pos * 2, first * 2 * sizeof(int16_t));
        if (first < to_copy)
        {
            std::memcpy(pcm + first * 2, _pcm_ring,
                       (to_copy - first) * 2 * sizeof(int16_t));
        }

        _ring_pos = (_ring_pos + to_copy) % _ring_cap;
        _ring_frames -= to_copy;

        _diag_printf("ring=%d/%d", (int)_ring_frames, (int)_ring_cap);
        return (int)to_copy;
    }

}
