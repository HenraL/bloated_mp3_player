/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* .......................
* ...><>.............<><.
* ..><>.><>.......<><.<><
* .><>.<><.><>.<><.<><.<>
* ..><>.><>.......<><.<><
* ...><>.............<><.
* .......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: audio.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Makes the speaker vibrate in ways that technically constitute sound.
* The bitrate is a suggestion, the fidelity is a memory, and the
* distortion is a feature.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: I2S audio output implementation.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/audio.hpp"
#include <driver/i2s.h>

static Audio::State state = Audio::State::Stopped;
static uint8_t volume = 128;
static i2s_port_t i2s_port = I2S_NUM_0;

bool Audio::begin(uint8_t bclk, uint8_t lrc, uint8_t dout)
{
    i2s_config_t i2s_config = {};
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.sample_rate = 22050;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    i2s_config.dma_buf_count = 8;
    i2s_config.dma_buf_len = 256;

    i2s_pin_config_t pin_config = {};
    pin_config.bck_io_num = bclk;
    pin_config.ws_io_num = lrc;
    pin_config.data_out_num = dout;
    pin_config.data_in_num = I2S_PIN_NO_CHANGE;

    esp_err_t err = i2s_driver_install(i2s_port, &i2s_config, 0, NULL);
    if (err != ESP_OK) return false;
    err = i2s_set_pin(i2s_port, &pin_config);
    if (err != ESP_OK) return false;
    i2s_zero_dma_buffer(i2s_port);
    state = State::Stopped;
    return true;
}

void Audio::set_volume(uint8_t vol)
{
    volume = vol;
}

uint8_t Audio::get_volume()
{
    return volume;
}

bool Audio::play(const char *wav_path)
{
    (void)wav_path;
    state = State::Error;
    return false;
}

bool Audio::play_raw(const int16_t *samples, size_t count)
{
    if (state == State::Playing) return false;
    if (!samples || count == 0) return false;
    state = State::Playing;
    size_t written = 0;
    size_t to_write = count * sizeof(int16_t);
    int16_t *buf = (int16_t *)malloc(to_write);
    if (!buf) { state = State::Error; return false; }
    for (size_t i = 0; i < count; i++)
        buf[i] = (samples[i] * volume) >> 8;
    i2s_write(i2s_port, buf, to_write, &written, portMAX_DELAY);
    free(buf);
    state = State::Stopped;
    return written == to_write;
}

void Audio::stop()
{
    i2s_zero_dma_buffer(i2s_port);
    state = State::Stopped;
}

void Audio::pause()
{
    if (state == State::Playing)
        state = State::Paused;
}

void Audio::resume()
{
    if (state == State::Paused)
        state = State::Playing;
}

Audio::State Audio::get_state()
{
    return state;
}

bool Audio::is_playing()
{
    return state == State::Playing;
}

void Audio::tick()
{
}

uint32_t Audio::get_position_ms()
{
    return 0;
}

uint32_t Audio::get_duration_ms()
{
    return 0;
}
