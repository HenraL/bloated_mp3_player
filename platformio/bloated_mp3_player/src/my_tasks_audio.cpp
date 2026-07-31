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
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {

        void audio(void *pvParameters)
        {
            SharedInstances::serial.serial_print("[Audio] So long, and thanks for all the fish.");
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(25);
            uint16_t tick_count = 0;
            bool fault_reported = false;
            SharedInstances::audio.play();

            while (true) {
                PROFILE_BLOCK("audio_tick");

                if (SharedInstances::audio.output_faulted() && !fault_reported) {
                    fault_reported = true;
                    SharedInstances::serial.serial_print(
                        "[Audio] I2S output stalled (err 0x%lx) -- DMA not draining, no sound",
                        (unsigned long)SharedInstances::audio.last_error()
                    );
                }

                if (tick_count % 50 == 0) {
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_AUDIO_STACK_HIGH_WATER,
                        "[Audio] Stack high-water mark: %u bytes free",
                        (unsigned int)uxTaskGetStackHighWaterMark(NULL)
                    );
                }
                tick_count++;

                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    if (SharedInstances::player.is_loaded()) {
                        int tick_r = SharedInstances::player.tick();
                        if (tick_r <= 0) {
                            SharedInstances::serial.serial_print("[Audio] tick=%d [%s]", tick_r, SharedInstances::player.last_diag());
                        }
                    } else {
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_AUDIO_NOT_LOADED, "[Audio] No players are loaded.");
                    }
                } else {
                    SharedInstances::serial.serial_debug(My::Config::Debug::UART_AUDIO_NOT_PLAYING, "[Audio] Audio is not playing.");
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
