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
* LAST Modified: 21:15:28 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the ui section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <audio.hpp>
#include <screen.hpp>
#include <environmental.hpp>
#include "my/config.hpp"

#include "my/tasks.hpp"

namespace MyTasks
{
    // ─── UI Task ──────────────────────────────────────────────────────────
    void ui(void *pvParameters)
    {
        (void)pvParameters;
        TickType_t xLastWake = xTaskGetTickCount();
        const TickType_t freq = pdMS_TO_TICKS(33);

        while (true) {
            MySerial::serial_print("[UI] Mostly harmless.");
            SharedInstances::display.clear();
            SharedInstances::display.setFont(MyConfig::FONT_TITLE);
            SharedInstances::display.printAt("Bloated MP3 v1.0", 0, 12);
            SharedInstances::display.setFont(MyConfig::FONT_BODY);

            Environmental::Reading env;
            if (Environmental::read(env)) {
                SharedInstances::display.printAt(0, 24, "Temp: %.1f C", env.temperature);
                SharedInstances::display.printAt(0, 32, "Hum:  %.0f %%", env.humidity);
                SharedInstances::display.printAt(0, 40, "Pres: %.0f hPa", env.pressure);
            }

            SharedInstances::display.printAt(0, 50, "Uptime: %lus", millis() / 1000);

            if (Audio::is_playing()) {
                SharedInstances::display.printAt(76, 24, ">> PLAY");
            } else {
                SharedInstances::display.printAt(76, 24, "|| STOP");
            }

            SharedInstances::display.drawRect(0, 54, 128, 8);
            SharedInstances::display.fillRect(2, 56, ((millis() / 100) % 100) * 124 / 100, 4);

            SharedInstances::display.display();
            vTaskDelayUntil(&xLastWake, freq);
        }
    }
} // namespace MyTasks
