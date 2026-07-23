#include "internal/player.hpp"
#include "internal/wav_decoder.hpp"
#include "internal/mp3_decoder.hpp"
#include <cstring>

namespace Audio
{

    Player::Player(Audio &audio)
        : _audio(audio)
        , _decoder(nullptr)
        , _loading(false)
    {
    }

    Player::~Player()
    {
        unload();
    }

    static bool has_extension(const char *path, const char *ext)
    {
        const char *dot = std::strrchr(path, '.');
        if (!dot)
        {
            return false;
        }
        size_t plen = std::strlen(dot);
        size_t elen = std::strlen(ext);
        if (plen != elen)
        {
            return false;
        }
        for (size_t i = 0; i < plen; i++)
        {
            char a = dot[i];
            char b = ext[i];
            if (a >= 'A' && a <= 'Z') a += 32;
            if (b >= 'A' && b <= 'Z') b += 32;
            if (a != b)
            {
                return false;
            }
        }
        return true;
    }

    bool Player::load(const char *path)
    {
        _loading = true;
        unload();

        if (has_extension(path, ".mp3"))
        {
            _decoder = new Mp3Decoder();
        }
        else
        {
            _decoder = new WavDecoder();
        }

        if (!_decoder->open(path))
        {
            delete _decoder;
            _decoder = nullptr;
            _loading = false;
            return false;
        }

        _audio.setSampleRate(_decoder->sample_rate());
        _loading = false;
        return true;
    }

    void Player::unload()
    {
        if (_decoder)
        {
            _decoder->close();
            delete _decoder;
            _decoder = nullptr;
        }
    }

    void Player::tick()
    {
        if (!_decoder || _loading)
        {
            return;
        }

        int frames = _decoder->read(_tick_buf, PLAYER_MAX_FRAMES);

        if (frames == 0)
        {
            _audio.stop();
            unload();
            return;
        }

        if (_decoder->channels() == 1)
        {
            for (int i = frames - 1; i >= 0; i--)
            {
                _tick_buf[i * 2]     = _tick_buf[i];
                _tick_buf[i * 2 + 1] = _tick_buf[i];
            }
            _audio.write(_tick_buf, frames * 2);
        }
        else
        {
            _audio.write(_tick_buf, frames * 2);
        }

        if (_decoder->eof())
        {
            _audio.stop();
            unload();
        }
    }

    bool Player::is_loaded() const
    {
        return _decoder && _decoder->is_open();
    }

}
