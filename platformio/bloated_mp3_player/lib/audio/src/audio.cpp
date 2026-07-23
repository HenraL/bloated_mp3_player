#include "internal/audio.hpp"

namespace Audio
{

    Audio::Audio(
        uint8_t speaker_pin_1, uint8_t speaker_pin_2,
        uint8_t dma_buf_count, uint16_t dma_buf_len)
        : _speaker_pin(speaker_pin_1)
        , _mirror_pin(speaker_pin_2)
        , _dma_buf_count(dma_buf_count)
        , _dma_buf_len(dma_buf_len)
        , _i2s_port(I2S_NUM_0)
    {
    }

    bool Audio::open()
    {
        if (_i2s_installed)
        {
            return true;
        }

        i2s_config_t cfg = {};
        cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_PDM);
        cfg.sample_rate = _sr;
        cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
        cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        cfg.dma_buf_count = _dma_buf_count;
        cfg.dma_buf_len = _dma_buf_len;
        cfg.use_apll = true;
        cfg.tx_desc_auto_clear = true;
        cfg.fixed_mclk = 0;
        cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;

        if (i2s_driver_install(_i2s_port, &cfg, 0, NULL) != ESP_OK)
        {
            return false;
        }
        _i2s_installed = true;

        i2s_pin_config_t pins = {};
        pins.bck_io_num = I2S_PIN_NO_CHANGE;
        pins.ws_io_num = I2S_PIN_NO_CHANGE;
        pins.data_out_num = _speaker_pin;
        pins.data_in_num = I2S_PIN_NO_CHANGE;

        if (i2s_set_pin(_i2s_port, &pins) != ESP_OK)
        {
            i2s_driver_uninstall(_i2s_port);
            _i2s_installed = false;
            return false;
        }

        gpio_matrix_out(
            _mirror_pin,
            i2s_periph_signal[_i2s_port].data_out_sig,
            false, false
        );

        return true;
    }

    void Audio::play()
    {
        if (_status == Playing) return;
        _status = Playing;
    }

    void Audio::pause()
    {
        _status = Paused;
    }

    void Audio::stop()
    {
        _status = Stopped;
        i2s_stop(_i2s_port);
        i2s_zero_dma_buffer(_i2s_port);
    }

    void Audio::setSampleRate(uint32_t sr)
    {
        _sr = sr;
        if (_i2s_installed)
        {
            i2s_set_clk(_i2s_port, _sr, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
        }
    }

    size_t Audio::write(const int16_t *samples, size_t count)
    {
        if (_status != Playing || !_i2s_installed)
        {
            return 0;
        }

        size_t frames = count / 2;
        size_t written = 0;

        while (written < frames)
        {
            size_t chunk = frames - written;
            if (chunk > 512)
            {
                chunk = 512;
            }

            for (size_t i = 0; i < chunk; i++)
            {
                size_t idx = (written + i) * 2;
                int32_t l = samples[idx];
                int32_t r = samples[idx + 1];
                int32_t m = (l + r) >> 1;
                m = (m * (int32_t)_volume) >> VOLUME_SHIFT;
                _mix_buf[i] = (int16_t)m;
            }

            size_t bytes_written = 0;
            i2s_write(_i2s_port, _mix_buf, chunk * sizeof(int16_t), &bytes_written, portMAX_DELAY);
            written += chunk;
        }

        return count;
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
