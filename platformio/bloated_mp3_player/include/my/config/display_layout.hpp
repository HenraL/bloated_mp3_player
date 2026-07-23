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
* FILE: display_layout.hpp
* CREATION DATE: 22-07-2026
* LAST Modified: 3:31:41 23-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the configuration for the elements that are going to be deisplayed on the screen, this aim's to allow easy configuration and placements of the elements.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
namespace My
{
    namespace Config
    {
        namespace DisplayLayout
        {
            // Title position
            static const int16_t                                    TITLE_X = 0;
            static const int16_t                                    TITLE_Y = 5;

            static const int16_t          ENVIRONEMENT_POSITION_Y = TITLE_Y + 8; //18
            // temperature position
            static const int16_t                              TEMPERATURE_X = 2;
            static const int16_t        TEMPERATURE_Y = ENVIRONEMENT_POSITION_Y;

            // Humidity position
            static const int16_t                                HUMIDITY_X = 45;
            static const int16_t           HUMIDITY_Y = ENVIRONEMENT_POSITION_Y;

            // Pressure position
            static const int16_t                                PRESSURE_X = 75;
            static const int16_t           PRESSURE_Y = ENVIRONEMENT_POSITION_Y;

            // Audio status message
            static const int16_t                            AUDIO_STATUS_X = 70;
            static const int16_t   AUDIO_STATUS_Y = ENVIRONEMENT_POSITION_Y + 8; //20

            // Uptime message
            static const int16_t                                   UPTIME_X = 0;
            static const int16_t                 UPTIME_Y = AUDIO_STATUS_Y + 25; //45

            // Rectangle
            static const int16_t                                RECTANGLE_X = 0;
            static const int16_t                     RECTANGLE_Y = UPTIME_Y + 9; //54
            static const int16_t                              RECTANGLE_W = 128;
            static const int16_t                                RECTANGLE_H = 8;

            // Rectangle fill
            static const int16_t                           RECTANGLE_FILL_X = 2;
            static const int16_t             RECTANGLE_FILL_Y = RECTANGLE_Y + 2; //56
            static const int16_t                           RECTANGLE_FILL_W = 0;
            static const int16_t                           RECTANGLE_FILL_H = 4;
        } // namespace DisplayLayout

    } // namespace Config

} // namespace My
