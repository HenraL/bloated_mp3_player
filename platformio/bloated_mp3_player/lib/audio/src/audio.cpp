#include "internal/audio.hpp"
#include <rom/gpio.h>
#include <soc/gpio_sig_map.h>

namespace Audio
{

    Audio::Audio(
        uint8_t bclk, uint8_t lrc, uint8_t dout, uint8_t dout2,
        uint8_t dma_buf_count, uint16_t dma_buf_len)
        : _bclk(bclk)
        , _lrc(lrc)
        , _dout(dout)
        , _dout2(dout2)
        , _dma_buf_count(dma_buf_count)
        , _dma_buf_len(dma_buf_len)
    {
    }

    bool Audio::open()
    {
        i2s_config_t cfg = {};
        cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        cfg.sample_rate = _sr;
        cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
        cfg.dma_buf_count = _dma_buf_count;
        cfg.dma_buf_len = _dma_buf_len;

        i2s_pin_config_t pins = {};
        pins.bck_io_num = _bclk;
        pins.ws_io_num = _lrc;
        pins.data_out_num = _dout;
        pins.data_in_num = I2S_PIN_NO_CHANGE;

        esp_err_t err = i2s_driver_install(_i2s_port, &cfg, 0, NULL);
        if (err != ESP_OK)
        {
            return false;
        }
        err = i2s_set_pin(_i2s_port, &pins);
        if (err != ESP_OK)
        {
            return false;
        }

        gpio_matrix_out(_dout2, I2S0O_SD1_OUT_IDX, false, false);

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
            scaled[i] = (samples[i] * _volume) >> 8;
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
