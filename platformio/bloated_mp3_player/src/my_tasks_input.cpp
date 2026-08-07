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
* LAST Modified: 23:40:19 06-08-2026
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
#include <profiling.hpp>
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
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_AUDIO_PATH, "[INPUT] filepath: %s", ti->path);
                // Do not stop() here: keep I2S running across track changes so
                // the previous track's tail drains out and the next track's PCM
                // appends seamlessly, without an unreliable restart cycle.
                SharedInstances::player.load(ti->path);
                SharedInstances::audio.play();
            }
        }

        static void handle_rotary_raw(void)
        {
            uint8_t a = 0;
            uint8_t b = 0;
            uint8_t sw = 0;
            if (Rotary::raw_changed(a, b, sw)) {
                SharedInstances::serial.serial_debug(
                    My::Config::Debug::UART_STICK_RAW,
                    "[INPUT] RAW A=%u B=%u", a, b
                );
                SharedInstances::serial.serial_debug(
                    My::Config::Debug::UART_STICK_RAW_SW,
                    "[INPUT] RAW SW=%u", sw
                );
            }
        }

        static void handle_rotary_volume(void)
        {
            int8_t dir = Rotary::get_direction();
            if (dir != 0) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_DIRECTION, "[INPUT] Clicky potentiometer value: %d", dir);
                uint8_t vol = SharedInstances::audio.getVolume();
                if (dir > 0) {
                    if (vol <= (My::Config::AUDIO_VOLUME_MAX - My::Config::AUDIO_VOLUME_STEP)) {
                        vol += My::Config::AUDIO_VOLUME_STEP;
                    } else {
                        vol = My::Config::AUDIO_VOLUME_MAX;
                    }
                } else {
                    if (vol >= My::Config::AUDIO_VOLUME_STEP) {
                        vol -= My::Config::AUDIO_VOLUME_STEP;
                    } else {
                        vol = My::Config::AUDIO_VOLUME_MIN;
                    }
                }
                SharedInstances::audio.setVolume(vol);
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_VOLUME, "[INPUT] Volume: %u", vol);
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

        static void handle_rotary_double_click(uint32_t *track_index, uint32_t *total_track_count)
        {
            if (Rotary::was_double_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_DOUBLE_PRESSED, "[INPUT] The rotary switch was double pressed, skipping track.");
                if (*total_track_count > 0) {
                    *track_index = (*track_index + 1) % *total_track_count;
                    play_track(track_index);
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
                PROFILE_BLOCK("input_tick");
                Rotary::tick();
                Ultrasonic::gesture_tick();

                handle_rotary_raw();
                handle_rotary_volume();
                handle_audio_play_pause();
                handle_rotary_double_click(&track_index, &total_track_count);
                handle_ultrasonic_press();
                handle_ultrasonic_swipe();

                if (SharedInstances::player.track_finished()) {
                    SharedInstances::serial.serial_print("[INPUT] Track finished, advancing to the next track.");
                    total_track_count = SDCard::total_tracks();
                    track_index = (track_index + 1) % total_track_count;
                    play_track(&track_index);
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
