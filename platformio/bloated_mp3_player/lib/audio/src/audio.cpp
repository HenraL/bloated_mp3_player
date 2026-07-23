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
        , _ring(nullptr)
        , _ring_mask(RING_CAPACITY - 1)
        , _ring_wp(0)
        , _ring_rp(0)
        , _output_task(nullptr)
    {
    }

    Audio::~Audio()
    {
        stop();
        if (_ring)
        {
            free(_ring);
            _ring = nullptr;
        }
    }

    bool Audio::open()
    {
        if (_i2s_installed)
        {
            return true;
        }

        if (!_ring)
        {
            size_t bytes = RING_CAPACITY * sizeof(int16_t);
            _ring = (int16_t*)heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM);
            if (!_ring)
            {
                _ring = (int16_t*)malloc(bytes);
            }
            if (!_ring)
            {
                return false;
            }
            _ring_wp = 0;
            _ring_rp = 0;
            _ring_mask = RING_CAPACITY - 1;
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

        if (_speaker_pin != _mirror_pin)
        {
            pinMode(_mirror_pin, OUTPUT);
            gpio_matrix_out(
                _mirror_pin,
                i2s_periph_signal[_i2s_port].data_out_sig,
                false, false
            );
        }

        return true;
    }

    void Audio::play()
    {
        if (_status == Playing) return;
        _status = Playing;

        if (!_output_task)
        {
            xTaskCreatePinnedToCore(
                _output_task_entry, "audio_out", 2048,
                this, 5, &_output_task, 1
            );
        }
    }

    void Audio::pause()
    {
        _status = Paused;
    }

    void Audio::stop()
    {
        _status = Stopped;

        if (_output_task)
        {
            vTaskDelete(_output_task);
            _output_task = nullptr;
        }

        if (_i2s_installed)
        {
            i2s_stop(_i2s_port);
            i2s_zero_dma_buffer(_i2s_port);
        }
    }

    void Audio::setSampleRate(uint32_t sr)
    {
        _sr = sr;
        if (_i2s_installed)
        {
            i2s_set_clk(_i2s_port, _sr, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
        }
    }

    size_t Audio::_ring_avail() const
    {
        return (_ring_wp - _ring_rp) & _ring_mask;
    }

    size_t Audio::_ring_space() const
    {
        return RING_CAPACITY - 1 - _ring_avail();
    }

    void Audio::_ring_write(const int16_t *data, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            _ring[_ring_wp] = data[i];
            _ring_wp = (_ring_wp + 1) & _ring_mask;
        }
    }

    size_t Audio::_ring_read(int16_t *buf, size_t count)
    {
        size_t avail = _ring_avail();
        if (count > avail)
        {
            count = avail;
        }
        for (size_t i = 0; i < count; i++)
        {
            buf[i] = _ring[_ring_rp];
            _ring_rp = (_ring_rp + 1) & _ring_mask;
        }
        return count;
    }

    size_t Audio::write(const int16_t *samples, size_t count)
    {
        if (_status != Playing || !_ring)
        {
            return 0;
        }

        size_t frames = count / 2;
        size_t written = 0;

        while (written < frames)
        {
            size_t space = _ring_space();
            if (space == 0)
            {
                taskYIELD();
                continue;
            }

            size_t chunk = frames - written;
            if (chunk > space)
            {
                chunk = space;
            }

            int16_t mix_buf[512];
            size_t mix_chunk = chunk > 512 ? 512 : chunk;
            if (mix_chunk < chunk)
            {
                chunk = mix_chunk;
            }

            for (size_t i = 0; i < chunk; i++)
            {
                size_t idx = (written + i) * 2;
                int32_t l = samples[idx];
                int32_t r = samples[idx + 1];
                int32_t m = ((int32_t)((l + r) >> 1) * _volume) >> VOLUME_SHIFT;
                mix_buf[i] = (int16_t)m;
            }

            _ring_write(mix_buf, chunk);
            written += chunk;
        }

        return count;
    }

    void Audio::_output_task_entry(void *arg)
    {
        Audio *self = (Audio *)arg;
        self->_output_task_loop();
    }

    void Audio::_output_task_loop()
    {
        int16_t out_buf[512];

        while (_status == Playing)
        {
            size_t avail = _ring_avail();
            if (avail == 0)
            {
                vTaskDelay(1);
                continue;
            }

            size_t chunk = avail > 512 ? 512 : avail;
            size_t n = _ring_read(out_buf, chunk);
            if (n == 0) continue;

            size_t to_send = n * sizeof(int16_t);
            uint8_t *ptr = (uint8_t *)out_buf;
            while (to_send > 0)
            {
                size_t written;
                esp_err_t err = i2s_write(
                    _i2s_port, ptr, to_send, &written, portMAX_DELAY
                );
                if (err != ESP_OK || written == 0) break;
                to_send -= written;
                ptr += written;
            }
        }

        vTaskDelete(NULL);
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
