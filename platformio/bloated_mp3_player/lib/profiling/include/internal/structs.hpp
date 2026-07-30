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
* FILE: structs.hpp
* CREATION DATE: 30-07-2026
* LAST Modified: 30-07-2026
* DESCRIPTION:
* POD types for the Profiler library.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Data structures used by the Profiler.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "constants.hpp"

namespace ProfilerData
{
    struct StackFrame
    {
        const char *name;
        uint64_t start_us;
    };

    struct TaskStack
    {
        TaskHandle_t handle;
        StackFrame frames[ProfilerConstants::MAX_DEPTH];
        uint16_t depth;
    };
}
