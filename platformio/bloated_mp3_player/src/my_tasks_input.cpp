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
* FILE: my_tasks_input.cpp
* CREATION DATE: 22-07-2026
* LAST Modified: 18:7:30 22-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the task in charge of handling input modules and take action on based on the input.
* // AR
* +==== END Bloated MP3 Player =================+
*/
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
            if (!SDCard::is_mounted()) {
                SharedInstances::serial.serial_print("[Input] sd card is not mounted.");
                return;
            }
            uint32_t n = SDCard::total_tracks();
            if (n == 0) {
                SharedInstances::serial.serial_print("[Input] There are no tracks to play.");
                return;
            }
            if (idx >= n) {
                SharedInstances::serial.serial_print("[Input] we have played all the tracks and will reset the current track index.");
                idx = 0;
            }
            track_index = idx;
            const SDCard::TrackInfo *ti = SDCard::get_track(track_index);
            if (ti) {
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

            while (true) {
                Rotary::tick();
                Ultrasonic::gesture_tick();

                int8_t dir = Rotary::get_direction();
                if (dir != 0) {
                    uint32_t n = SDCard::total_tracks();
                    if (n > 0) {
                        if (dir > 0) {
                            track_index = (track_index + 1) % n;
                        } else {
                            if (track_index == 0) {
                                track_index = n - 1;
                            } else {
                                track_index -= 1;
                            }
                        }
                        play_track(track_index);
                    }
                }

                if (Rotary::was_pressed()) {
                    SharedInstances::serial.serial_print("[Input] The rotary switch was pressed.");
                    if (SharedInstances::audio.getStatus() == Audio::Playing) {
                        SharedInstances::audio.pause();
                    } else {
                        SharedInstances::audio.play();
                    }
                }

                if (Ultrasonic::is_pressed()) {
                    SharedInstances::serial.serial_print("[Input] ultrasonic is pressed.");
                    if (SharedInstances::audio.getStatus() == Audio::Playing) {
                        SharedInstances::audio.pause();
                    } else {
                        SharedInstances::audio.play();
                    }
                }

                int8_t swipe = Ultrasonic::get_swipe_dir();
                if (swipe > 0) {
                    SharedInstances::serial.serial_print("[Input] Ultrasonic was swiped.");
                    SharedInstances::audio.stop();
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
