#include <rotary.hpp>
#include <sdcard.hpp>
#include <ultrasonic.hpp>
#include "my/tasks.hpp"
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {
        static uint32_t track_index = 0;

        static void play_track(uint32_t idx)
        {
            if (!SDCard::is_mounted())
            {
                return;
            }
            uint32_t n = SDCard::total_tracks();
            if (n == 0)
            {
                return;
            }
            if (idx >= n)
            {
                idx = 0;
            }
            track_index = idx;
            const SDCard::TrackInfo *ti = SDCard::get_track(track_index);
            if (ti)
            {
                SharedInstances::audio.stop();
                SharedInstances::player.load(ti->path);
                SharedInstances::audio.play();
            }
        }

        void input(void *pvParameters)
        {
            SharedInstances::serial.serial_print("[Input] Don't Panic.");
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(10);

            while (true)
            {
                Rotary::tick();
                Ultrasonic::gesture_tick();

                int8_t dir = Rotary::get_direction();
                if (dir != 0)
                {
                    uint32_t n = SDCard::total_tracks();
                    if (n > 0)
                    {
                        if (dir > 0)
                        {
                            track_index = (track_index + 1) % n;
                        }
                        else
                        {
                            track_index = (track_index == 0) ? n - 1 : track_index - 1;
                        }
                        play_track(track_index);
                    }
                }

                if (Rotary::was_pressed())
                {
                    if (SharedInstances::audio.getStatus() == Audio::Playing)
                    {
                        SharedInstances::audio.pause();
                    }
                    else
                    {
                        SharedInstances::audio.play();
                    }
                }

                if (Ultrasonic::is_pressed())
                {
                    SharedInstances::audio.stop();
                }

                int8_t swipe = Ultrasonic::get_swipe_dir();
                if (swipe > 0)
                {
                    SharedInstances::audio.stop();
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
