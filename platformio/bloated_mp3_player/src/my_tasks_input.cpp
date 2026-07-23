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
* LAST Modified: 15:17:51 23-07-2026
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

        static void play_track(uint32_t *track_index)
        {
            uint32_t n = 0;
            const SDCard::TrackInfo *ti = NULL;

            if (!SDCard::is_mounted()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_MOUNTED, "[INPUT] sd card is not mounted.");
                return;
            }
            n = SDCard::total_tracks();
            if (n == 0) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_NO_TRACKS, "[INPUT] There are no tracks to play.");
                return;
            }
            if (*track_index >= n) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_ALL_TRACKS_PLAYED, "[INPUT] we have played all the tracks and will reset the current track index.");
                *track_index = 0;
            }
            SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_CURRENT_TRACK_INDEX, "[INPUT] Playing track %lu", *track_index);
            ti = SDCard::get_track(*track_index);
            if (ti) {
                SharedInstances::audio.stop();
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_AUDIO_PATH, "[INPUT] filepath: %s", ti->path);
                SharedInstances::player.load(ti->path);
                SharedInstances::audio.play();
            }
        }

        static void handle_rotary_rotation(uint32_t total_track_count, uint32_t *track_index)
        {
            int8_t dir = Rotary::get_direction();
            if (dir != 0) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_DIRECTION, "[INPUT] Clicky potentiometer value: %d", dir);
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_TOTAL_TRACKS, "[INPUT] SD Total tracks: %lu", total_track_count);
                if (total_track_count > 0) {
                    if (dir > 0) {
                        *track_index = (*track_index + 1) % total_track_count;
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_TRACK_INDEX, "[INPUT] track_index=%lu", *track_index);
                    } else {
                        if (*track_index == 0) {
                            *track_index = total_track_count - 1;
                        } else {
                            *track_index -= 1;
                        }
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_TRACK_INDEX, "[INPUT] track_index=%lu", *track_index);
                    }
                    play_track(track_index);
                }
            } else {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_DIRECTION_ZERO, "[INPUT] The clicky pot has not been turned.");
            }
        }

        static void handle_audio_play_pause(void)
        {
            if (Rotary::was_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_PRESSED, "[INPUT] The rotary switch was pressed.");
                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::audio.pause();
                } else {
                    SharedInstances::audio.play();
                }
            }
        }

        static void handle_ultrasonic_press(void)
        {
            if (Ultrasonic::is_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_PRESSED, "[INPUT] ultrasonic is pressed.");
                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::audio.pause();
                } else {
                    SharedInstances::audio.play();
                }
            }
        }

        static void handle_ultrasonic_swipe(void)
        {
            int8_t swipe = Ultrasonic::get_swipe_dir();
            SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_SWIPE_VALUE, "[INPUT] Ultrasonic swipe value: %d", swipe);
            if (swipe > 0) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_SWIPED, "[INPUT] Ultrasonic was swiped.");
                SharedInstances::audio.stop();
            }
        }

        void input(void *pvParameters)
        {
            TickType_t xLastWake = xTaskGetTickCount();
            TickType_t freq = pdMS_TO_TICKS(10);
            uint32_t total_track_count = SDCard::total_tracks();
            uint32_t track_index = 0;

            SharedInstances::serial.serial_print("[INPUT] Don't Panic.");
            play_track(&track_index);

            while (true) {
                Rotary::tick();
                Ultrasonic::gesture_tick();

                handle_rotary_rotation(total_track_count, &track_index);
                handle_audio_play_pause();
                handle_ultrasonic_press();
                handle_ultrasonic_swipe();

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
