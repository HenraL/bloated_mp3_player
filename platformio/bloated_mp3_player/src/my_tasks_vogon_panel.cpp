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
* FILE: my_tasks_vogon_panel.cpp
* CREATION DATE: 07-08-2026
* LAST Modified: 00:00:00 00-00-0000
* DESCRIPTION:
* Drives the Vogon panel (the second 1601B). Walks the PROGMEM banner
* table the exact same way the onboard LED task walks its morse tables:
* one frame per tick, dwell time per frame, an end-of-message blip, and
* a {nullptr, 0} sentinel that loops the sequence.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Vogon panel task implementation.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <stdint.h>
#include <profiling.hpp>
#include "my/tasks.hpp"
#include "my/infos.hpp"
#include "shared_instances.hpp"
#include "my/config/vogon_panel.hpp"

namespace My
{
    namespace Tasks
    {
        // ─── Vogon panel helpers ─────────────────────────────────────────────
        static inline void vogon_advance_message(uint16_t *banner_index)
        {
            uint16_t next = *banner_index + 1;
            if (My::Config::vogon_messages[next].steps == nullptr) {
                *banner_index = 0;
            } else {
                *banner_index = next;
            }
        }

        void vogon_panel(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            uint16_t banner_index = 0;
            uint16_t frame_index = 0;
            uint16_t duration_ms = 0;
            uint16_t nxt = 0;
            uint8_t cur_i2c_error = 0;
            uint8_t last_i2c_error = 0;
            bool in_eom = false;
            SharedInstances::serial.serial_print(My::Infos::vogon_quote, My::Infos::quote_ships_bricks);

            // Static title; the banner scroll animates on the bottom row.
            SharedInstances::char_lcd_vogon.print_at(0, 0, "VOGON POETRY");

            while (true) {
                PROFILE_BLOCK("vogon_tick");
                duration_ms = 0;
                xLastWake = xTaskGetTickCount();

                if (in_eom) {
                    // Briefly unroll the interlude, then get back to the stanza.
                    duration_ms = My::Config::vogon_eom[0].steps[frame_index].dwell_ms;
                    SharedInstances::char_lcd_vogon.print_at(
                        0,
                        1,
                        My::Config::vogon_eom[0].steps[frame_index].text
                    );
                    nxt = frame_index + 1;
                    if (nxt >= My::Config::vogon_eom[0].length) {
                        frame_index = 0;
                        in_eom = false;
                    } else {
                        frame_index = nxt;
                    }
                } else {
                    // Show the current banner frame, then advance.
                    duration_ms = My::Config::vogon_messages[banner_index].steps[frame_index].dwell_ms;
                    SharedInstances::char_lcd_vogon.print_at(
                        0,
                        1,
                        My::Config::vogon_messages[banner_index].steps[frame_index].text
                    );
                    nxt = frame_index + 1;
                    if (nxt >= My::Config::vogon_messages[banner_index].length) {
                        frame_index = 0;
                        vogon_advance_message(&banner_index);
                        in_eom = true; // turn on the interlude for the next stanza
                        SharedInstances::serial.serial_debug(
                            My::Config::Debug::UART_VOGON_EOM_ENTER,
                            My::Infos::vogon_eom_enter
                        );
                        SharedInstances::serial.serial_debug(
                            My::Config::Debug::UART_VOGON_STANZA_SWITCH,
                            My::Infos::vogon_stanza_switch,
                            banner_index
                        );
                    } else {
                        frame_index = nxt;
                    }
                }

                // Report I2C trouble with the Vogon panel only when it
                // appears or changes, so a dead bus yields one line, not spam.
                cur_i2c_error = SharedInstances::char_lcd_vogon.last_i2c_error();
                if (cur_i2c_error != last_i2c_error) {
                    last_i2c_error = cur_i2c_error;
                    if (cur_i2c_error != 0) {
                        SharedInstances::serial.serial_debug(
                            My::Config::Debug::UART_VOGON_I2C_ERROR,
                            My::Infos::vogon_i2c_error,
                            cur_i2c_error,
                            (unsigned int)My::Config::CHAR_LCD2_I2C_ADDR
                        );
                    }
                }

                vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(duration_ms));
            }
        }
    } // namespace Tasks
} // namespace My
