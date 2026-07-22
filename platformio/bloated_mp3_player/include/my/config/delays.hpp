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
* FILE: delays.hpp
* CREATION DATE: 20-07-2026
* LAST Modified: 21:14:52 22-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will contain the imposed delays required during initialisation and runtime.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>

namespace My
{
    namespace Config
    {
        namespace Delays
        {
            // Initialisation delay for the serial instance (as well as for the dev to connect to the serial output)
            static const uint32_t SERIAL_INITIALISATION_DELAY = 500; //ms
            static const uint32_t PRE_THREAD_INITIALISATION_DELAY = 1000; //ms

            // How often to poll the AHT20+BMP280 environmental sensor (milliseconds)
            static const uint32_t ENVIRONMENTAL_POLL_INTERVAL_MS = 1000; //ms
        } // namespace Delays
    } // namespace Config
} // namespace My

