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
* LAST Modified: 19:35:19 31-07-2026
* DESCRIPTION:
* Per-task call stack profiler. Each trace_begin pushes a frame onto
* the current task's stack; trace_end pops it and outputs a
* PROFILING: line through the configured output callback.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Live embedded profiling with flamegraph export.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/profiling.hpp"
#include <esp_timer.h>
#include <stdio.h>

ProfilerData::TaskStack Profiler::_task_stacks[ProfilerConstants::MAX_TRACKED_TASKS];
uint16_t Profiler::_task_count = 0;
Profiler::output_func_t Profiler::_output = nullptr;
bool Profiler::_enabled = true;

void Profiler::set_output(output_func_t func)
{
    _output = func;
}

void Profiler::set_enabled(bool en)
{
    _enabled = en;
}

ProfilerData::TaskStack *Profiler::_get_task_stack()
{
    TaskHandle_t current = xTaskGetCurrentTaskHandle();
    for (uint16_t i = 0; i < _task_count; i++) {
        if (_task_stacks[i].handle == current) {
            return &_task_stacks[i];
        }
    }
    if (_task_count >= ProfilerConstants::MAX_TRACKED_TASKS) {
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
    if (!_enabled) return UINT16_MAX;
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
    if (!_enabled) return;
    if (slot >= ProfilerConstants::MAX_DEPTH) return;
    ProfilerData::TaskStack *ts = _get_task_stack();
    if (ts == nullptr) return;
    if (slot >= ts->depth) return;

    uint64_t dur = esp_timer_get_time() - ts->frames[slot].start_us;
    char line[128];
    uint16_t pos = 0;

    static const char header[] = "PROFILING: ";
    for (uint16_t i = 0; header[i] != '\0' && pos < sizeof(line) - 24; i++) {
        line[pos++] = header[i];
    }
    for (uint16_t i = 0; i <= slot && pos < sizeof(line) - 24; i++) {
        if (i > 0) {
            line[pos++] = ';';
            if (pos >= sizeof(line) - 24) break;
        }
        const char *n = ts->frames[i].name;
        while (*n != '\0' && pos < sizeof(line) - 24) {
            line[pos++] = *n;
            n++;
        }
    }

    line[pos++] = ' ';
    char digits[24];
    uint8_t ndigits = 0;
    uint64_t v = dur;
    if (v == 0) {
        digits[ndigits++] = '0';
    }
    while (v > 0 && ndigits < sizeof(digits)) {
        digits[ndigits++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (ndigits > 0 && pos < sizeof(line) - 1) {
        line[pos++] = digits[--ndigits];
    }
    line[pos] = '\0';

    if (_output != nullptr) {
        _output(line);
    } else {
        Serial.println(line);
    }
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

    for (UBaseType_t i = 0; i < count; i++) {
        uint32_t pct_x100 = total_time > 0
            ? (uint32_t)((uint64_t)tasks[i].ulRunTimeCounter * 10000 / total_time)
            : 0;
        char line[128];
        snprintf(line, sizeof(line), "PROFILING: %s %lu", tasks[i].pcTaskName, (unsigned long)pct_x100);
        if (_output != nullptr) {
            _output(line);
        } else {
            Serial.println(line);
        }
    }
    free(tasks);
#else
    if (_output != nullptr) {
        _output("PROFILING: Enable CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS "
            "and CONFIG_FREERTOS_USE_TRACE_FACILITY in build_flags "
            "for task-level CPU stats");
    } else {
        Serial.println("PROFILING: Enable CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS "
            "and CONFIG_FREERTOS_USE_TRACE_FACILITY in build_flags "
            "for task-level CPU stats");
    }
#endif
}
