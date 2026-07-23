#include "internal/mp3_decoder.hpp"
#include <sdcard.hpp>
#include <cstring>

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
        , _frame_samples(0)
        , _frame_consumed(0)
    {
    }

    Mp3Decoder::~Mp3Decoder()
    {
        close();
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
        _frame_samples = 0;
        _frame_consumed = 0;

        return true;
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
        _frame_samples = 0;
        _frame_consumed = 0;
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

    int Mp3Decoder::read(int16_t *pcm, size_t max_frames)
    {
        if (!_file || _eof)
        {
            return 0;
        }

        size_t total_frames = 0;

        while (total_frames < max_frames)
        {
            if (_frame_consumed < _frame_samples)
            {
                int remaining = _frame_samples - _frame_consumed;
                int needed = max_frames - total_frames;
                int copy_frames = (remaining < needed) ? remaining : needed;
                int copy_samples = copy_frames * _channels;
                int src_offset = _frame_consumed * _channels;

                std::memcpy(pcm + total_frames * _channels,
                           _frame_pcm + src_offset,
                           copy_samples * sizeof(int16_t));

                total_frames += copy_frames;
                _frame_consumed += copy_frames;
                continue;
            }

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
                refill();
                mp3_ptr = _in_buf + _in_pos;
                mp3_left = _in_avail;
                if (mp3_left < (int)MP3_SYNC_MIN)
                {
                    break;
                }
            }

            int result = MP3Decode(_decoder, &mp3_ptr, &mp3_left, _frame_pcm, 1);

            int consumed = _in_avail - mp3_left;
            _in_pos += consumed;
            _in_avail -= consumed;

            if (result != 0)
            {
                continue;
            }

            MP3FrameInfo info;
            MP3GetLastFrameInfo(_decoder, &info);

            _sr = info.samprate;
            _channels = info.nChans;
            _frame_samples = info.outputSamps / info.nChans;
            _frame_consumed = 0;
        }

        return total_frames;
    }

}
