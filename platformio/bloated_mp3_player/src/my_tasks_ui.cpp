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
* FILE: my_tasks_ui.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 11:7:42 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the ui section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <audio.hpp>
#include <environmental.hpp>
#include "my/tasks.hpp"
#include "my/config.hpp"


namespace My
{
    namespace Tasks
    {
        // ─── UI Task ──────────────────────────────────────────────────────────
        void ui(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(33);

            while (true) {
                SharedInstances::serial.serial_print("[UI] Mostly harmless.");
                SharedInstances::lcd.clear();
                SharedInstances::lcd.setFont(My::Config::FONT_TITLE);
                SharedInstances::lcd.printAt("Bloated MP3 v1.0", 0, 12);
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);

                Environmental::Reading env;
                if (Environmental::read(env)) {
                    SharedInstances::lcd.printAt(0, 24, "Temp: %.1f C", env.temperature);
                    SharedInstances::lcd.printAt(0, 32, "Hum:  %.0f %%", env.humidity);
                    SharedInstances::lcd.printAt(0, 40, "Pres: %.0f hPa", env.pressure);
                }

                SharedInstances::lcd.printAt(0, 50, "Uptime: %lus", millis() / 1000);

                if (Audio::is_playing()) {
                    SharedInstances::lcd.printAt(76, 24, ">> PLAY");
                } else {
                    SharedInstances::lcd.printAt(76, 24, "|| STOP");
                }

                SharedInstances::lcd.drawRect(0, 54, 128, 8);
                SharedInstances::lcd.fillRect(2, 56, ((millis() / 100) % 100) * 124 / 100, 4);

                SharedInstances::lcd.display();
                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
