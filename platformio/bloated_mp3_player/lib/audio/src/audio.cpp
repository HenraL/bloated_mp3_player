#include "internal/audio.hpp"

namespace Audio
{

    Audio::Audio(
        uint8_t speaker_pin_1, uint8_t speaker_pin_2,
        uint8_t dma_buf_count, uint16_t dma_buf_len)
        : _speaker_pin_1(speaker_pin_1)
        , _speaker_pin_2(speaker_pin_2)
        , _dma_buf_count(dma_buf_count)
        , _dma_buf_len(dma_buf_len)
    {
    }

    bool Audio::open()
    {
        i2s_config_t cfg = {};
        cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);
        cfg.sample_rate = _sr;
        cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        cfg.communication_format = I2S_COMM_FORMAT_STAND_MSB;
        cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        cfg.dma_buf_count = _dma_buf_count;
        cfg.dma_buf_len = _dma_buf_len;

        esp_err_t err = i2s_driver_install(_i2s_port, &cfg, 0, NULL);
        if (err != ESP_OK)
        {
            return false;
        }

        err = i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
        if (err != ESP_OK)
        {
            return false;
        }

        i2s_zero_dma_buffer(_i2s_port);
        _status = Stopped;
        return true;
    }

    void Audio::play()
    {
        _status = Playing;
    }

    void Audio::pause()
    {
        if (_status == Playing)
        {
            _status = Paused;
        }
    }

    void Audio::stop()
    {
        i2s_zero_dma_buffer(_i2s_port);
        _status = Stopped;
    }

    void Audio::setSampleRate(uint32_t sr)
    {
        _sr = sr;
        i2s_set_clk(_i2s_port, sr, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
    }

    size_t Audio::write(const int16_t *samples, size_t count)
    {
        size_t todo = count;
        if (todo > 2048)
        {
            todo = 2048;
        }

        int16_t scaled[2048];
        for (size_t i = 0; i < todo; i++)
        {
            int32_t s = (samples[i] * _volume) >> 8;
            uint8_t dac_val = (s + 32768) >> 8;
            scaled[i] = (int16_t)((uint16_t)dac_val << 8);
        }

        size_t written = 0;
        i2s_write(_i2s_port, scaled, todo * sizeof(int16_t), &written, portMAX_DELAY);
        return written / sizeof(int16_t);
    }

    void Audio::setVolume(uint8_t v)
    {
        _volume = v;
    }

    uint8_t Audio::getVolume() const
    {
        return _volume;
    }

    Status Audio::getStatus() const
    {
        return _status;
    }

} // namespace Audio
