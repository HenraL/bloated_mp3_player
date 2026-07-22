#include "internal/player.hpp"
#include <sdcard.hpp>

namespace Audio
{

    Player::Player(Audio &audio)
        : _audio(audio)
    {
    }

    bool Player::load(const char *path)
    {
        unload();

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

        if (memcmp(hdr.riff, "RIFF", 4) != 0 ||
            memcmp(hdr.wave, "WAVE", 4) != 0)
        {
            f.close();
            return false;
        }

        uint32_t pos   = sizeof(hdr);
        uint32_t fsize = SDCard::size(f);
        bool     found = false;

        while (pos + 8 <= fsize)
        {
            char     chunk_id[4];
            uint32_t chunk_size;

            SDCard::seek(f, pos);
            SDCard::read(f, (uint8_t *)chunk_id, 4);
            SDCard::read(f, (uint8_t *)&chunk_size, 4);

            if (memcmp(chunk_id, "data", 4) == 0)
            {
                _data_start = pos + 8;
                _data_left  = chunk_size;
                SDCard::seek(f, _data_start);
                found = true;
                break;
            }

            pos += 8 + ((chunk_size + 1) & ~1);
        }

        if (!found)
        {
            f.close();
            return false;
        }

        _audio.setSampleRate(hdr.sample_rate);
        _file = f;
        return true;
    }

    void Player::unload()
    {
        if (_file)
        {
            _file.close();
        }
        _data_start = 0;
        _data_left  = 0;
    }

    void Player::tick()
    {
        if (!_file)
        {
            return;
        }

        int16_t mono_buf[1024];
        size_t to_read = _data_left;
        if (to_read > sizeof(mono_buf))
        {
            to_read = sizeof(mono_buf);
        }

        size_t bytes_read = SDCard::read(_file, (uint8_t *)mono_buf, to_read);

        if (bytes_read == 0)
        {
            _audio.stop();
            unload();
            return;
        }

        size_t mono_count = bytes_read / sizeof(int16_t);
        int16_t stereo[2048];

        for (size_t i = 0; i < mono_count; i++)
        {
            stereo[i * 2]     = mono_buf[i];
            stereo[i * 2 + 1] = mono_buf[i];
        }

        _audio.write(stereo, mono_count * 2);
        _data_left -= bytes_read;

        if (_data_left == 0 || bytes_read < to_read)
        {
            _audio.stop();
            unload();
        }
    }

    bool Player::is_loaded() const
    {
        return _file ? true : false;
    }

} // namespace Audio
