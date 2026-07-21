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
* CREATION DATE: 17-07-2026
* LAST Modified: 11:4:28 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the input section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <audio.hpp>
#include <rotary.hpp>
#include <ultrasonic.hpp>
#include "my/tasks.hpp"
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {
        // ─── Input Task ───────────────────────────────────────────────────────
        void input(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(10);

            while (true) {
                SharedInstances::serial.serial_print("[Input] Don't Panic.");
                Rotary::tick();
                Ultrasonic::gesture_tick();

                int8_t dir = Rotary::get_direction();
                if (dir != 0) {
                    if (dir > 0)
                        Matrix::set_animation(
                            (Matrix::Animation)(((uint8_t)Matrix::get_animation() + 1)
                                % (uint8_t)Matrix::Animation::Count));
                    else
                        Matrix::set_animation(
                            (Matrix::Animation)(((uint8_t)Matrix::get_animation()
                                - 1 + (uint8_t)Matrix::Animation::Count)
                                % (uint8_t)Matrix::Animation::Count));
                }

                if (Rotary::was_pressed()) {
                    if (Audio::is_playing())
                        Audio::pause();
                    else
                        Audio::resume();
                }

                if (Ultrasonic::is_pressed()) {
                    Audio::stop();
                }

                int8_t swipe = Ultrasonic::get_swipe_dir();
                if (swipe > 0) {
                    if (Audio::is_playing()) {
                        Audio::stop();
                        // skip track
                    }
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My

