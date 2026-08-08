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
* LAST Modified: 09-08-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the task in charge of handling input modules and taking
* action based on the input. Playback order is folder-first: the first
* audio of the first folder, then the rest of that folder, then the next
* folder (see My::TrackBrowser).
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include <rotary.hpp>
#include <sdcard.hpp>
#include <ultrasonic.hpp>
#include <profiling.hpp>
#include "my/tasks.hpp"
#include "my/infos.hpp"
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {
        static void play_browser_position(void)
        {
            const char *path = SharedInstances::track_browser.track_path();
            if (!SDCard::is_mounted()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_MOUNTED, My::Infos::input_sd_not_mounted);
                return;
            }
            if (!path || !path[0]) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_NO_TRACKS, My::Infos::input_no_tracks);
                return;
            }
            SharedInstances::serial.serial_debug(
                My::Config::Debug::UART_SD_CURRENT_TRACK_INDEX,
                My::Infos::input_playing_track,
                (unsigned long)SharedInstances::track_browser.track_index() + 1
            );
            SharedInstances::serial.serial_debug(My::Config::Debug::UART_SD_AUDIO_PATH, My::Infos::input_filepath, path);
            // Do not stop() here: keep I2S running across track changes so
            // the previous track's tail drains out and the next track's PCM
            // appends seamlessly, without an unreliable restart cycle.
            SharedInstances::player.load(path);
            SharedInstances::audio.play();
        }

        static void handle_rotary_volume(void)
        {
            int8_t dir = Rotary::get_direction();
            if (dir != 0) {
                if (SharedInstances::track_browser.is_browsing()) {
                    // While browsing, the knob walks the folder list or the
                    // track list, depending on the selection stage.
                    if (SharedInstances::track_browser.in_track_stage()) {
                        SharedInstances::track_browser.move_track(dir);
                        SharedInstances::serial.serial_debug(
                            My::Config::Debug::UART_SD_CURRENT_TRACK_INDEX,
                            My::Infos::input_select_moved,
                            (unsigned long)SharedInstances::track_browser.track_index() + 1,
                            (unsigned long)SharedInstances::track_browser.folder_track_count()
                        );
                    } else {
                        SharedInstances::track_browser.move_folder(dir);
                        SharedInstances::serial.serial_debug(
                            My::Config::Debug::UART_SD_CURRENT_TRACK_INDEX,
                            My::Infos::input_select_moved,
                            (unsigned long)SharedInstances::track_browser.folder_index() + 1,
                            (unsigned long)SharedInstances::track_browser.folder_count()
                        );
                    }
                    return;
                }
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_DIRECTION, My::Infos::input_clicky_value, dir);
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
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_VOLUME, My::Infos::input_volume, vol);
            }
        }

        static void handle_audio_play_pause(void)
        {
            if (Rotary::was_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_PRESSED, My::Infos::input_switch_pressed);
                if (SharedInstances::track_browser.in_folder_stage()) {
                    // First click: step into the selected folder to browse
                    // its tracks.
                    SharedInstances::track_browser.enter_folder();
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_SD_CURRENT_TRACK_INDEX,
                        My::Infos::input_select_start
                    );
                } else if (SharedInstances::track_browser.in_track_stage()) {
                    // Second click: play the highlighted track of the album.
                    SharedInstances::track_browser.picked();
                    play_browser_position();
                } else if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::audio.pause();
                } else {
                    SharedInstances::audio.play();
                }
            }
        }

        static void handle_rotary_double_click(void)
        {
            if (Rotary::was_double_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_DOUBLE_PRESSED, My::Infos::input_double_pressed);
                if (SharedInstances::track_browser.in_track_stage()) {
                    // Double click inside a folder: back to the albums.
                    SharedInstances::track_browser.back_to_folders();
                } else if (SharedInstances::track_browser.in_folder_stage()) {
                    SharedInstances::track_browser.move_folder(1);
                } else {
                    SharedInstances::track_browser.move_track(1);
                    play_browser_position();
                }
            }
        }

        static void handle_rotary_triple_click(void)
        {
            if (Rotary::was_triple_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_STICK_TRIPLE_PRESSED, My::Infos::input_triple_pressed);
                if (SharedInstances::track_browser.in_track_stage()) {
                    SharedInstances::track_browser.back_to_folders();
                } else if (SharedInstances::track_browser.in_folder_stage()) {
                    SharedInstances::track_browser.move_folder(-1);
                } else {
                    SharedInstances::track_browser.move_track(-1);
                    play_browser_position();
                }
            }
        }

        static void handle_ultrasonic_press(void)
        {
            if (Ultrasonic::is_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_PRESSED, My::Infos::input_ultrasonic_pressed);
                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::audio.pause();
                } else {
                    SharedInstances::audio.play();
                }
            }
        }

        static void handle_ultrasonic_double_wave(void)
        {
            if (Ultrasonic::is_double_pressed()) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_DOUBLE_WAVED, My::Infos::input_ultrasonic_double_waved);
                // Two quick waves: back to the menu (browse the albums) no
                // matter what the player was doing.
                SharedInstances::audio.stop();
                SharedInstances::track_browser.begin();
                SharedInstances::serial.serial_debug(
                    My::Config::Debug::UART_ULTRASONIC_DOUBLE_WAVED,
                    My::Infos::input_going_back_to_menu
                );
            }
        }

        static void handle_ultrasonic_swipe(void)
        {
            int8_t swipe = Ultrasonic::get_swipe_dir();
            SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_SWIPE_VALUE, My::Infos::input_swipe_value, swipe);
            if (swipe > 0) {
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_ULTRASONIC_SWIPED, My::Infos::input_swiped);
                SharedInstances::audio.stop();
            }
        }

        void input(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            TickType_t freq = pdMS_TO_TICKS(10);

            SharedInstances::serial.serial_print(My::Infos::input_dont_panic);
            SharedInstances::track_browser.begin();
            if (SharedInstances::track_browser.is_browsing()) {
                SharedInstances::serial.serial_print(My::Infos::input_select_start);
            } else {
                SharedInstances::serial.serial_print(My::Infos::input_no_tracks);
            }

            while (true) {
                PROFILE_BLOCK("input_tick");
                Rotary::tick();
                Ultrasonic::gesture_tick();

                handle_rotary_volume();
                handle_audio_play_pause();
                handle_rotary_double_click();
                handle_rotary_triple_click();
                handle_ultrasonic_press();
                handle_ultrasonic_double_wave();
                handle_ultrasonic_swipe();

                if (SharedInstances::player.track_finished()) {
                    SharedInstances::serial.serial_print(My::Infos::input_track_finished);
                    SharedInstances::track_browser.move_track(1);
                    play_browser_position();
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My