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
* FILE: my_tasks_led.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 11:53:10 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the led section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <stdint.h>
#include "my/tasks.hpp"
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {

        // ─── LED Task ─────────────────────────────────────────────────────────
        static inline void play_special_animation(uint16_t *character_index, bool *special_animation)
        {
            uint16_t next = *character_index + 1;
            if (next >= My::Config::onboard_eom[0].length) {
                *character_index = 0;
                *special_animation = false;
            } else {
                *character_index = next;
            }
        }

        static inline void step_animation(uint16_t *sentence_index, uint16_t *character_index, bool *special_animation)
        {
            uint16_t next = *character_index + 1;
            if (next >= My::Config::onboard_messages[*sentence_index].length) {
                *character_index = 0;
                if (My::Config::onboard_messages[*sentence_index + 1].steps == nullptr) {
                    *sentence_index = 0;
                } else {
                    *sentence_index += 1;
                    *special_animation = true;
                }
            } else {
                *character_index = next;
            }
        }

        void led(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            uint16_t sentence_index = 0;
            uint16_t character_index = 0;
            uint16_t led_position = 0;
            uint16_t duration_ms = 0;
            bool refresh = true;
            bool special_animation = false;
            SharedInstances::serial.serial_print("[LED] The light that burns twice as bright...");

            while (true) {
                duration_ms = 0;
                xLastWake = xTaskGetTickCount();

                if (special_animation) {
                    duration_ms = My::Config::onboard_eom[0].steps[character_index].duration_ms;
                    SharedInstances::onboard.set_led_position(
                        led_position,
                        My::LED::get_colour_from_pointer(&My::Config::onboard_eom[0].steps[character_index].colour),
                        duration_ms,
                        refresh
                    );
                    play_special_animation(&character_index, &special_animation);
                } else {
                    duration_ms = My::Config::onboard_messages[sentence_index].steps[character_index].duration_ms;
                    SharedInstances::onboard.set_led_position(
                        led_position,
                        My::LED::get_colour_from_pointer(&My::Config::onboard_messages[sentence_index].steps[character_index].colour),
                        duration_ms,
                        refresh
                    );
                    step_animation(&sentence_index, &character_index, &special_animation);
                }

                vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(duration_ms));
            }
        }
    } // namespace Tasks
} // namespace My
