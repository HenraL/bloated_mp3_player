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
            // Audio (8192 → 16384 — the Helix MP3 decode + FATFS/SDMMC read
            // chain is deep, and tick-interrupt frames on top of a mid-decode
            // stack blew the canary even at 8K)
            namespace Audio
            {
                static const char PROCESS_NAME[] = "Audio";
                static const uint32_t US_STACK_DEPTH = 16384;
                static const UBaseType_t TASK_PRIORITY = 3;
                static const BaseType_t X_CORE_ID = 0;
            } // namespace Audio
            // Sensor (2048 → 4096 — serial_debug with %a.1f floats pulls in
            // newlib _dtoa_r + malloc, which blew the canary at 2K)
            namespace Sensor
            {
                static const char PROCESS_NAME[] = "Sensors";
                static const uint32_t US_STACK_DEPTH = 4096;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 0;
            } // namespace Sensor
            // LED (stack increased from 2048 — set_led_position + NeoPixel
            // RMT show() + profiler trace_end blows the canary at 2K)
            namespace LED
            {
                static const char PROCESS_NAME[] = "LED";
                static const uint32_t US_STACK_DEPTH = 8192;
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
            // Vogon panel (second 1602A). Does its own slow thing on card 1,
            // the same core as the LED and UI so the I2C traffic is audio/free.
            // (2048 → 4096 — the IMU-angle print_at passes %5.1f floats
            // through vsnprintf + _dtoa_r + malloc, which blew the canary at 2K)
            namespace VogonPanel
            {
                static const char PROCESS_NAME[] = "VogonPanel";
                static const uint32_t US_STACK_DEPTH = 4096;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 1;
            } // namespace VogonPanel
            // Character LCD (2004A info panel). Own slow task so the HD44780
            // writes never stall the 33ms UI refresh of the SPI screen.
            namespace CharLcd
            {
                static const char PROCESS_NAME[] = "CharLCDPanel";
                static const uint32_t US_STACK_DEPTH = 4096;
                static const UBaseType_t TASK_PRIORITY = 1;
                static const BaseType_t X_CORE_ID = 1;
            } // namespace CharLcd
        } // namespace Priorities
    } // namespace Config
} // namespace My
