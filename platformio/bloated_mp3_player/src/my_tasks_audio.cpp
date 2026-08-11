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
* FILE: my_tasks_audio.cpp
* CREATION DATE: 23-07-2026
* LAST Modified: 19:35:41 31-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the task in charge of handling the audio ouput loop (make the music play)
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include <audio.hpp>
#include <profiling.hpp>
#include "my/tasks.hpp"
#include "my/infos.hpp"
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {

        void audio(void *pvParameters)
        {
            SharedInstances::serial.serial_print(My::Infos::UART::audio_so_long_fish);
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(25);
            uint16_t tick_count = 0;
            bool fault_reported = false;
            Audio::Status last_status = Audio::Stopped;
            float tone_phase = 0.0f;
            SharedInstances::audio.play();

            while (true) {
                PROFILE_BLOCK("audio_tick");

                Audio::Status status = SharedInstances::audio.getStatus();
                if (status != last_status) {
                    SharedInstances::serial.serial_print(
                        My::Infos::UART::audio_status_change,
                        status == Audio::Playing ? "playing"
                        : status == Audio::Paused  ? "paused"
                        :                            "stopped"
                    );
                    last_status = status;
                }

                if (SharedInstances::audio.output_faulted() && !fault_reported) {
                    fault_reported = true;
                    SharedInstances::serial.serial_print(
                        My::Infos::UART::audio_i2s_stalled,
                        (unsigned long)SharedInstances::audio.last_error()
                    );
                }

                if (tick_count % 50 == 0) {
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_AUDIO_STACK_HIGH_WATER,
                        My::Infos::UART::audio_stack_hwm,
                        (unsigned int)uxTaskGetStackHighWaterMark(NULL)
                    );
                }
                tick_count++;

                if (status == Audio::Playing && My::Config::Debug::AUDIO_TEST_TONE_ENABLED) {
                    int16_t tone[2304];
                    for (int i = 0; i < 1152; i++) {
                        int16_t v = (int16_t)((float)My::Config::Debug::AUDIO_TEST_TONE_AMPLITUDE * sinf(tone_phase));
                        tone[i * 2] = v;
                        tone[i * 2 + 1] = v;
                        tone_phase += 0.062665f;
                        if (tone_phase > 6.2831853f) {
                            tone_phase -= 6.2831853f;
                        }
                    }
                    SharedInstances::audio.write(tone, 2304);
                } else if (status == Audio::Playing) {
                    if (SharedInstances::player.is_loaded()) {
                        int tick_r = SharedInstances::player.tick();
                        if (tick_r <= 0) {
                            SharedInstances::serial.serial_print(My::Infos::UART::audio_tick_failed, tick_r, SharedInstances::player.last_diag());
                        }
                    } else {
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_AUDIO_NOT_LOADED, My::Infos::UART::audio_no_player);
                    }
                } else {
                    SharedInstances::serial.serial_debug(My::Config::Debug::UART_AUDIO_NOT_PLAYING, My::Infos::UART::audio_not_playing);
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
