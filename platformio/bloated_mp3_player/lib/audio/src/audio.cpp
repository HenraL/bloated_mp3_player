#include "internal/audio.hpp"

namespace Audio
{

    static Audio *g_audio_instance = nullptr;

    void IRAM_ATTR Audio::_isr_handler()
    {
        Audio *self = g_audio_instance;
        if (!self || self->_status != Playing)
        {
            return;
        }

        size_t rp = self->_ring_rp;
        size_t wp = self->_ring_wp;
        size_t mask = AUDIO_RING_SIZE - 1;
        size_t avail = (wp - rp) & mask;

        if (avail >= 2)
        {
            int16_t left = self->_ring[rp];
            rp = (rp + 1) & mask;
            int16_t right = self->_ring[rp];
            rp = (rp + 1) & mask;
            self->_ring_rp = rp;

            int32_t vl = (int32_t)left * self->_volume;
            int32_t vr = (int32_t)right * self->_volume;
            uint32_t dl = ((vl >> VOLUME_SHIFT) + (int32_t)PCM_OFFSET) >> PCM_SHIFT;
            uint32_t dr = ((vr >> VOLUME_SHIFT) + (int32_t)PCM_OFFSET) >> PCM_SHIFT;

            ledc_set_duty(LEDC_LOW_SPEED_MODE, self->_ledc_chan_1, dl);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, self->_ledc_chan_1);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, self->_ledc_chan_2, dr);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, self->_ledc_chan_2);
        }
    }

    Audio::Audio(
        uint8_t speaker_pin_1, uint8_t speaker_pin_2,
        uint8_t dma_buf_count, uint16_t dma_buf_len)
        : _speaker_pin_1(speaker_pin_1)
        , _speaker_pin_2(speaker_pin_2)
        , _dma_buf_count(dma_buf_count)
        , _dma_buf_len(dma_buf_len)
        , _ring_wp(0)
        , _ring_rp(0)
        , _timer(nullptr)
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

        _ring_wp = 0;
        _ring_rp = 0;
        _status = Stopped;
        _timer = NULL;

        g_audio_instance = this;
        return true;
    }

    void Audio::play()
    {
        if (_status == Playing) return;
        if (_sr == 0) return;

        _status = Playing;
        _ring_wp = 0;
        _ring_rp = 0;

        uint64_t alarm = 1000000 / _sr;
        if (alarm < 10) alarm = 10;

        if (_timer)
        {
            timerAlarmDisable(_timer);
            timerDetachInterrupt(_timer);
            timerEnd(_timer);
        }
        _timer = timerBegin(1, 80, true);
        if (_timer)
        {
            timerAttachInterrupt(_timer, &Audio::_isr_handler, true);
            timerAlarmWrite(_timer, alarm, true);
            timerAlarmEnable(_timer);
        }
    }

    void Audio::pause()
    {
        if (_status == Playing)
        {
            _status = Paused;
            if (_timer)
            {
                timerAlarmDisable(_timer);
            }
        }
    }

    void Audio::stop()
    {
        if (_timer)
        {
            timerAlarmDisable(_timer);
        }
        ledc_set_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_1, STOP_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_1);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_2, STOP_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, _ledc_chan_2);
        _status = Stopped;
    }

    void Audio::setSampleRate(uint32_t sr)
    {
        _sr = sr;
        if (_timer && _status == Playing)
        {
            uint64_t alarm = 1000000 / _sr;
            if (alarm < 10) alarm = 10;
            timerAlarmWrite(_timer, alarm, true);
        }
    }

    size_t Audio::_ring_avail() const
    {
        return (_ring_wp - _ring_rp) & (AUDIO_RING_SIZE - 1);
    }

    size_t Audio::write(const int16_t *samples, size_t count)
    {
        size_t written = 0;
        size_t mask = AUDIO_RING_SIZE - 1;

        while (written < count)
        {
            size_t avail = _ring_avail();
            size_t space = (AUDIO_RING_SIZE - 1) - avail;
            if (space == 0)
            {
                taskYIELD();
                continue;
            }

            size_t chunk = count - written;
            if (chunk > space) chunk = space;

            size_t wp = _ring_wp;
            size_t first = AUDIO_RING_SIZE - wp;
            if (first > chunk) first = chunk;

            memcpy(_ring + wp, samples + written, first * sizeof(int16_t));
            if (first < chunk)
            {
                memcpy(_ring, samples + written + first, (chunk - first) * sizeof(int16_t));
            }

            size_t new_wp = (wp + chunk) & mask;
            _ring_wp = new_wp;
            written += chunk;
        }

        return written;
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
