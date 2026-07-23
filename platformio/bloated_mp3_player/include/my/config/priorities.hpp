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
* FILE: priorities.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:10:19 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will contain the priority indexes for the tasks created.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once
#include <stdint.h>
#include <FreeRTOS.h>
namespace My
{
    namespace Config
    {
        namespace Priorities
        {
            // UI
            namespace UI
            {
                static const char PROCESS_NAME[] = "UI";
                static const uint32_t US_STACK_DEPTH = 4096;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 1;
            } // namespace UI
            // Audio (stack increased from 2048 — scan_tracks + WavHeader parsing
            // blows the canary at 2K)
            namespace Audio
            {
                static const char PROCESS_NAME[] = "Audio";
                static const uint32_t US_STACK_DEPTH = 8192;
                static const UBaseType_t TASK_PRIORITY = 3;
                static const BaseType_t X_CORE_ID = 0;
            } // namespace Audio
            // Sensor
            namespace Sensor
            {
                static const char PROCESS_NAME[] = "Sensors";
                static const uint32_t US_STACK_DEPTH = 2048;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 0;
            } // namespace Sensor
            // LED
            namespace LED
            {
                static const char PROCESS_NAME[] = "LED";
                static const uint32_t US_STACK_DEPTH = 2048;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 1;
            } // namespace LED
            // Matrix
            namespace Matrix
            {
                static const char PROCESS_NAME[] = "Matrix";
                static const uint32_t US_STACK_DEPTH = 2048;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 1;
            } // namespace Matrix
            // Input
            namespace Input
            {
                static const char PROCESS_NAME[] = "Input";
                static const uint32_t US_STACK_DEPTH = 4096;
                static const UBaseType_t TASK_PRIORITY = 2;
                static const BaseType_t X_CORE_ID = 0;
            } // namespace Input
            // Serial
            namespace Serial
            {
                static const char PROCESS_NAME[] = "SerialOut";
                static const uint32_t US_STACK_DEPTH = 2048;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 1;
            } // namespace Serial
        } // namespace Priorities
    } // namespace Config
} // namespace My
