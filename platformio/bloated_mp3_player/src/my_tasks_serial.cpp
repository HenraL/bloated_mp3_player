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
* FILE: my_tasks_serial.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:31:48 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file containing the function in charge of outputting the storred buffer to the UART terminal.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "my/tasks.hpp"
#include "shared_instances.hpp"

namespace My
{
    namespace Tasks
    {
        void serial(void *pvParameters)
        {
            (void)pvParameters;
            char msg[SERIAL_MSG_LEN * sizeof(char)];
            while (true) {
                SharedInstances::serial.get_queue(&msg);
                Serial.println(msg);
            }
        }
    }
}
