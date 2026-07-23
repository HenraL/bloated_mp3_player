#include "internal/wav_decoder.hpp"
#include <sdcard.hpp>
#include <cstring>

namespace Audio
{

    bool WavDecoder::open(const char *path)
    {
        close();

        File f = SDCard::open(path);
        if (!f)
        {
            return false;
        }

        WavHeader hdr;
        if (SDCard::read(f, (uint8_t *)&hdr, sizeof(hdr)) != sizeof(hdr))
        {
            f.close();
            return false;
        }

        if (std::memcmp(hdr.riff, "RIFF", WAV_CHUNK_ID_LEN) != 0 ||
            std::memcmp(hdr.wave, "WAVE", WAV_CHUNK_ID_LEN) != 0)
        {
            f.close();
            return false;
        }

        uint32_t pos   = sizeof(hdr);
        uint32_t fsize = SDCard::size(f);
        bool     found = false;

        while (pos + WAV_CHUNK_HEADER_LEN <= fsize)
        {
            char     chunk_id[4];
            uint32_t chunk_size;

            SDCard::seek(f, pos);
            SDCard::read(f, (uint8_t *)chunk_id, WAV_CHUNK_ID_LEN);
            SDCard::read(f, (uint8_t *)&chunk_size, WAV_CHUNK_ID_LEN);

            if (std::memcmp(chunk_id, "data", WAV_CHUNK_ID_LEN) == 0)
            {
                _data_start = pos + 8;
                _data_left  = chunk_size;
                SDCard::seek(f, _data_start);
                found = true;
                break;
            }

            pos += WAV_CHUNK_HEADER_LEN + ((chunk_size + WAV_CHUNK_ALIGN) & ~WAV_CHUNK_ALIGN);
        }

        if (!found)
        {
            f.close();
            return false;
        }

        _sr = hdr.sample_rate;
        _channels = hdr.channels;
        _eof = false;
        _file = f;
        return true;
    }

    void WavDecoder::close()
    {
        if (_file)
        {
            _file.close();
        }
        _data_start = 0;
        _data_left = 0;
        _sr = 0;
        _channels = 0;
        _eof = false;
    }

    int WavDecoder::read(int16_t *pcm, size_t max_frames)
    {
        if (!_file || _eof)
        {
            return 0;
        }

        size_t max_bytes = max_frames * _channels * sizeof(int16_t);
        size_t to_read = _data_left;
        if (to_read > max_bytes)
        {
            to_read = max_bytes;
        }

        size_t bytes_read = SDCard::read(_file, (uint8_t *)pcm, to_read);
        if (bytes_read == 0)
        {
            _eof = true;
            return 0;
        }

        size_t frames = bytes_read / (_channels * sizeof(int16_t));
        _data_left -= bytes_read;

        if (_data_left == 0)
        {
            _eof = true;
        }

        return frames;
    }

}
