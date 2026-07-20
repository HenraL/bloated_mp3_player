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
* LAST Modified: 12:12:21 20-07-2026
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
            // Initialisation delay for the serial instance
            static const uint32_t SERIAL_INITIALISATION_DELAY = 500; //ms
        } // namespace Delays

    } // namespace Config

} // namespace My

