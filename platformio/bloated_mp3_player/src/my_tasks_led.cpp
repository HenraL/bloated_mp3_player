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
* LAST Modified: 21:22:32 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the led section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "my/tasks.hpp"
#include "shared_instances.hpp"

namespace MyTasks
{

    // ─── LED Task ─────────────────────────────────────────────────────────
    void led(void *pvParameters)
    {
        (void)pvParameters;
        TickType_t xLastWake = xTaskGetTickCount();
        const TickType_t freq = pdMS_TO_TICKS(50);

        while (true) {
            SharedInstances::serial.serial_print("[LED] The light that burns twice as bright...");

            uint32_t wave = (millis() / 500) % 2;
            SharedInstances::onboard.setPixelColor(0,
                SharedInstances::onboard.Color(wave ? 0 : 8, wave ? 8 : 0, 0, 0)
            );
            SharedInstances::onboard.show();

            vTaskDelayUntil(&xLastWake, freq);
        }
    }
} // namespace MyTasks
