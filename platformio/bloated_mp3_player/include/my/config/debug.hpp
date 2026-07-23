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
* FILE: debug.hpp
* CREATION DATE: 23-07-2026
* LAST Modified: 15:19:56 23-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file in charge of providing the information wether to show a debug line or not.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
// Global debugging
#define ENABLE_GLOBAL_DEBUGGING
namespace My
{
    namespace Config
    {
        namespace Debug
        {
            // BMP280 debug lines
            static const bool UART_BMP280_RAW_MODULE_DATA = false;
            static const bool UART_BMP280_COMPUTED_MODULE_DATA = false;

            // Input debug lines
            // |- Clicky potentiometer
            static const bool UART_STICK_DIRECTION = true;
            static const bool UART_STICK_DIRECTION_ZERO = false;
            static const bool UART_STICK_TRACK_INDEX = true;
            static const bool UART_STICK_PRESSED = false;

            // |- Ultrasonic
            static const bool UART_ULTRASONIC_PRESSED = true;
            static const bool UART_ULTRASONIC_SWIPED = true;
            static const bool UART_ULTRASONIC_SWIPE_VALUE = false;

            // SD Card
            static const bool UART_SD_TOTAL_TRACKS = true;
            static const bool UART_SD_MOUNTED = true;
            static const bool UART_SD_NO_TRACKS = true;
            static const bool UART_SD_ALL_TRACKS_PLAYED = true;
            static const bool UART_SD_CURRENT_TRACK_INDEX = true;
            static const bool UART_SD_AUDIO_PATH = true;

            // Sensor (tilt)
            static const bool UART_ANGLE_SENSOR_SHAKE = true;
            static const bool UART_ANGLE_SENSOR_TILT_LEFT = true;
            static const bool UART_ANGLE_SENSOR_TILT_RIGHT = true;

            // Audio
            static const bool UART_AUDIO_NOT_PLAYING = true;
            static const bool UART_AUDIO_NOT_LOADED = true;
            static const bool UART_AUDIO_TICKED = true;

        } // namespace Debug

    } // namespace Config

} // namespace My
