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
* LAST Modified: 30-07-2026
* DESCRIPTION:
* Per-task call stack profiler. Each trace_begin pushes a frame onto
* the current task's stack; trace_end pops it and outputs a
* PROFILING: line with the full call path to the shared serial
* instance for flamegraph consumption.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Live embedded profiling with flamegraph export.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/profiling.hpp"
#include <esp_timer.h>
#include "shared_instances.hpp"

ProfilerData::TaskStack Profiler::_task_stacks[ProfilerConstants::MAX_TRACKED_TASKS];
uint16_t Profiler::_task_count = 0;

ProfilerData::TaskStack* Profiler::_get_task_stack()
{
    TaskHandle_t current = xTaskGetCurrentTaskHandle();
    for (uint16_t i = 0; i < _task_count; i++)
    {
        if (_task_stacks[i].handle == current)
        {
            return &_task_stacks[i];
        }
    }
    if (_task_count >= ProfilerConstants::MAX_TRACKED_TASKS)
    {
        return nullptr;
    }
    ProfilerData::TaskStack *ts = &_task_stacks[_task_count];
    ts->handle = current;
    ts->depth = 0;
    _task_count++;
    return ts;
}

uint16_t Profiler::trace_begin(const char *name)
{
    ProfilerData::TaskStack *ts = _get_task_stack();
    if (ts == nullptr) return UINT16_MAX;
    if (ts->depth >= ProfilerConstants::MAX_DEPTH) return UINT16_MAX;
    uint16_t slot = ts->depth;
    ts->frames[slot].name = name;
    ts->frames[slot].start_us = esp_timer_get_time();
    ts->depth++;
    return slot;
}

void Profiler::trace_end(uint16_t slot)
{
    if (slot >= ProfilerConstants::MAX_DEPTH) return;
    ProfilerData::TaskStack *ts = _get_task_stack();
    if (ts == nullptr) return;
    if (slot >= ts->depth) return;

    uint64_t dur = esp_timer_get_time() - ts->frames[slot].start_us;
    char path[ProfilerConstants::MAX_PATH_LEN];
    uint16_t pos = 0;
    for (uint16_t i = 0; i <= slot && pos < ProfilerConstants::MAX_PATH_LEN - 1; i++)
    {
        if (i > 0)
        {
            path[pos++] = ';';
            if (pos >= ProfilerConstants::MAX_PATH_LEN - 1) break;
        }
        const char *n = ts->frames[i].name;
        while (*n != '\0' && pos < ProfilerConstants::MAX_PATH_LEN - 1)
        {
            path[pos++] = *n;
            n++;
        }
    }
    path[pos] = '\0';
    SharedInstances::serial.serial_print("PROFILING: %s %llu", path, dur);
    ts->depth = slot;
}

void Profiler::dump_task_stats()
{
#if (configUSE_TRACE_FACILITY == 1) && (configGENERATE_RUN_TIME_STATS == 1)
    UBaseType_t count = uxTaskGetNumberOfTasks();
    TaskStatus_t *tasks = (TaskStatus_t *)malloc(count * sizeof(TaskStatus_t));
    if (tasks == nullptr) return;

    uint32_t total_time = 0;
    count = uxTaskGetSystemState(tasks, count, &total_time);

    for (UBaseType_t i = 0; i < count; i++)
    {
        uint32_t pct_x100 = total_time > 0
            ? (uint32_t)((uint64_t)tasks[i].ulRunTimeCounter * 10000 / total_time)
            : 0;
        SharedInstances::serial.serial_print(
            "PROFILING: %s %lu",
            tasks[i].pcTaskName,
            (unsigned long)pct_x100
        );
    }
    free(tasks);
#else
    SharedInstances::serial.serial_print(
        "PROFILING: Enable CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS "
        "and CONFIG_FREERTOS_USE_TRACE_FACILITY in build_flags "
        "for task-level CPU stats"
    );
#endif
}
