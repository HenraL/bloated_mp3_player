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
* FILE: profiling.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Implementation of the Profiler class. Collects manual trace points and
* FreeRTOS task statistics, then dumps them in a format suitable for
* flamegraph rendering.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Profiling data collection and serial export.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "profiling.hpp"
#include <esp_timer.h>

Profiler::TracePoint Profiler::_points[Profiler::MAX_TRACES];
uint16_t Profiler::_count = 0;

uint16_t Profiler::trace_begin(const char *name)
{
    if (_count >= MAX_TRACES) return UINT16_MAX;
    uint16_t slot = _count++;
    _points[slot].name     = name;
    _points[slot].start_us = esp_timer_get_time();
    _points[slot].end_us   = 0;
    _points[slot].active   = true;
    return slot;
}

void Profiler::trace_end(uint16_t slot)
{
    if (slot >= MAX_TRACES || !_points[slot].active) return;
    _points[slot].end_us  = esp_timer_get_time();
    _points[slot].active  = false;
}

void Profiler::dump_traces()
{
    Serial.println("=== PROFILING TRACES ===");
    for (uint16_t i = 0; i < _count; i++)
    {
        if (_points[i].end_us == 0) continue;
        uint64_t dur = _points[i].end_us - _points[i].start_us;
        Serial.printf("%s %llu\n", _points[i].name, dur);
    }
    Serial.println("=== END PROFILING TRACES ===");
}

void Profiler::dump_task_stats()
{
    Serial.println("=== PROFILING TASK STATS ===");
#if (configUSE_TRACE_FACILITY == 1) && (configGENERATE_RUN_TIME_STATS == 1)
    TaskStatus_t *tasks = nullptr;
    UBaseType_t count = uxTaskGetNumberOfTasks();
    tasks = (TaskStatus_t *)malloc(count * sizeof(TaskStatus_t));
    if (!tasks)
    {
        Serial.println("No mem");
        Serial.println("=== END PROFILING TASK STATS ===");
        return;
    }

    uint32_t total_time = 0;
    count = uxTaskGetSystemState(tasks, count, &total_time);

    for (UBaseType_t i = 0; i < count; i++)
    {
        float pct = total_time > 0
            ? 100.0f * tasks[i].ulRunTimeCounter / total_time
            : 0.0f;
        Serial.printf("%s %u\n", tasks[i].pcTaskName, (unsigned)(pct * 100));
    }
    free(tasks);
#else
    Serial.println("Enable CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS "
                   "and CONFIG_FREERTOS_USE_TRACE_FACILITY in "
                   "build_flags for task-level CPU stats");
#endif
    Serial.println("=== END PROFILING TASK STATS ===");
}
