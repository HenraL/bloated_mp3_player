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
* LAST Modified: 11:29:55 21-07-2026
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
            My::Config::Structures::duration_ms_t length = My::Config::onboard_eom[0].length;
            if (*character_index >= length) {
                *character_index = 0;
                *special_animation = false;
            } else {
                *character_index += 1;
            }
        }

        static inline void step_animation(uint16_t *sentence_index, uint16_t *character_index, bool *special_animation)
        {
            My::Config::Structures::LEDMessage node = My::Config::onboard_messages[*sentence_index];
            My::Config::Structures::duration_ms_t sentence_size = node.length;
            if (My::Config::onboard_messages[*sentence_index + 1].steps == nullptr) {
                *sentence_index = 0;
            } else {
                if (*character_index >= sentence_size) {
                    *character_index = 0;
                    *sentence_index += 1;
                    *special_animation = true;
                } else {
                    *character_index += 1;
                }
            }
        }

        void led(void *pvParameters)
        {
            uint32_t loop_delay = 50;
            uint32_t loop_delay_ms = loop_delay * 10;
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(loop_delay);
            uint32_t tick = 0;
            uint32_t animation_delay = 0;
            uint16_t sentence_index = 0;
            uint16_t character_index = 0;
            uint16_t led_position = 0;
            bool special_animation = false;
            bool refresh = true;
            SharedInstances::serial.serial_print("[LED] The light that burns twice as bright...");


            while (true) {
                tick = (millis() / animation_delay) % 2;
                if (tick == 0) {
                    if (special_animation == true) {
                        play_special_animation(&character_index, &special_animation);
                        animation_delay = My::Config::onboard_eom[0].steps[character_index].duration_ms;
                        SharedInstances::onboard.set_led_position(
                            led_position,
                            My::LED::get_colour_from_pointer(&My::Config::onboard_eom[0].steps[character_index].colour),
                            animation_delay,
                            refresh
                        );
                    } else {
                        step_animation(&sentence_index, &character_index, &special_animation);
                        animation_delay = My::Config::onboard_messages[sentence_index].steps[character_index].duration_ms;
                        SharedInstances::onboard.set_led_position(
                            led_position,
                            My::LED::get_colour_from_pointer(&My::Config::onboard_messages[sentence_index].steps[character_index].colour),
                            animation_delay,
                            refresh
                        );
                    }
                }
                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
