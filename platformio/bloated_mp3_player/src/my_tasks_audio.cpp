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
            const TickType_t freq = pdMS_TO_TICKS(20);

            while (true) {
                PROFILE_BLOCK("audio_tick");

                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    if (SharedInstances::player.is_loaded()) {
                        SharedInstances::player.tick();
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_AUDIO_TICKED, "[Audio] Player is loaded and has ticked.");
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
