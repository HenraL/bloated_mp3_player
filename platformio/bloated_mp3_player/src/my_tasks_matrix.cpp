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
* FILE: my_tasks_matrix.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 15:41:59 22-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the matrix section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "my/tasks.hpp"
#include "shared_instances.hpp"
#include <matrix.hpp>

namespace My
{
    namespace Tasks
    {
        // ─── Matrix Task ─────────────────────────────────────────────────────────
        void matrix(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(50);
            SharedInstances::serial.serial_print("[MATRIX] The light that burns twice as bright...");

            while (true) {
                Matrix::tick();
                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
