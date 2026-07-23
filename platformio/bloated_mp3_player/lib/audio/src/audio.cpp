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
        (void)_dma_buf_count;
        (void)_dma_buf_len;
    }

    bool Audio::open()
    {
        ledc_timer_config_t timer = {};
        timer.speed_mode = LEDC_LOW_SPEED_MODE;
        timer.duty_resolution = LEDC_TIMER_8_BIT;
        timer.timer_num = LEDC_TIMER_0;
        timer.freq_hz = PWM_FREQ_HZ;
        timer.clk_cfg = LEDC_AUTO_CLK;
        if (ledc_timer_config(&timer) != ESP_OK)
        {
            return false;
        }

        ledc_channel_config_t chan1 = {};
        chan1.gpio_num = _speaker_pin_1;
        chan1.speed_mode = LEDC_LOW_SPEED_MODE;
        chan1.channel = LEDC_CHANNEL_0;
        chan1.intr_type = LEDC_INTR_DISABLE;
        chan1.timer_sel = LEDC_TIMER_0;
        chan1.duty = STOP_DUTY;
        chan1.hpoint = 0;
        if (ledc_channel_config(&chan1) != ESP_OK)
        {
            return false;
        }
        _ledc_chan_1 = LEDC_CHANNEL_0;

        ledc_channel_config_t chan2 = {};
        chan2.gpio_num = _speaker_pin_2;
        chan2.speed_mode = LEDC_LOW_SPEED_MODE;
        chan2.channel = LEDC_CHANNEL_1;
        chan2.intr_type = LEDC_INTR_DISABLE;
        chan2.timer_sel = LEDC_TIMER_0;
        chan2.duty = STOP_DUTY;
        chan2.hpoint = 0;
        if (ledc_channel_config(&chan2) != ESP_OK)
        {
            return false;
        }
        _ledc_chan_2 = LEDC_CHANNEL_1;

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
        ledc_set_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_1, STOP_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_1);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_2, STOP_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_2);
        _status = Stopped;
    }

    void Audio::setSampleRate(uint32_t sr)
    {
        _sr = sr;
    }

    size_t Audio::write(const int16_t *samples, size_t count)
    {
        size_t frames = count / 2;
        uint32_t us_per_frame = (_sr > 0) ? US_PER_SEC / _sr : DEFAULT_US_PER_FRAME;

        for (size_t i = 0; i < frames; i++)
        {
            uint32_t t0 = micros();

            int32_t left = (samples[i * 2] * _volume) >> VOLUME_SHIFT;
            int32_t right = (samples[i * 2 + 1] * _volume) >> VOLUME_SHIFT;

            uint32_t duty_l = (left + PCM_OFFSET) >> PCM_SHIFT;
            uint32_t duty_r = (right + PCM_OFFSET) >> PCM_SHIFT;

            ledc_set_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_1, duty_l);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_1);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_2, duty_r);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_2);

            uint32_t elapsed = micros() - t0;
            if (elapsed < us_per_frame)
            {
                delayMicroseconds(us_per_frame - elapsed);
            }
        }

        return frames * 2;
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
