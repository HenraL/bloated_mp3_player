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
* FILE: threads.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:13:17 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that contains the instances for thread handling.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once

// The library in charge of dispatching threads
#include <FreeRTOS.h>
#include <task.h>

namespace My
{
    class Threads
    {
        public:
        Threads();
        ~Threads();
        // initialiser instances
        void initialise_ui();
        void initialise_audio();
        void initialise_sensors();
        void initialise_led();
        void initialise_matrix();
        void initialise_input();
        void initialise_serial();
        private:
        // ─── FreeRTOS task handles ────────────────────────────────────────────
        TaskHandle_t _handle_ui = nullptr;
        TaskHandle_t _handle_audio = nullptr;
        TaskHandle_t _handle_sensor = nullptr;
        TaskHandle_t _handle_led = nullptr;
        TaskHandle_t _handle_matrix = nullptr;
        TaskHandle_t _handle_input = nullptr;
        TaskHandle_t _handle_serial = nullptr;
    };
}
