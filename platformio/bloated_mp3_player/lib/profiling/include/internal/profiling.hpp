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
* FILE: profiling.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 30-07-2026
* DESCRIPTION:
* Performance profiler — RAII scoped timers with per-task call stack
* tracking. Outputs PROFILING: lines over the shared serial instance
* for consumption by the flamegraph pipeline.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Profiler class, ProfileBlock RAII helper, PROFILE_BLOCK macro.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include "structs.hpp"

class Profiler
{
public:
    typedef void (*output_func_t)(const char *msg);

    static uint16_t trace_begin(const char *name);
    static void     trace_end(uint16_t slot);
    static void     dump_task_stats();
    static void     set_output(output_func_t func);

private:
    Profiler() = delete;
    static ProfilerData::TaskStack _task_stacks[ProfilerConstants::MAX_TRACKED_TASKS];
    static uint16_t _task_count;
    static output_func_t _output;

    static ProfilerData::TaskStack* _get_task_stack();
};

class ProfileBlock
{
public:
    ProfileBlock(const char *name)
        : _slot(Profiler::trace_begin(name)) {}
    ~ProfileBlock() { Profiler::trace_end(_slot); }
private:
    uint16_t _slot;
};

#define PROFILE_BLOCK(name) ProfileBlock __profile_block_##__LINE__(name)
