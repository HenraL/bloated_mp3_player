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
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Public API for performance profiling. Use PROFILE_BLOCK(name) to
* measure a scope, or call Profiler::dump_task_stats() to export
* FreeRTOS task CPU utilisation in flamegraph-folded format.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Public interface for the Profiler singleton.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Profiler
{
public:
    struct TracePoint
    {
        const char *name;
        uint64_t start_us;
        uint64_t end_us;
        bool active;
    };

    static constexpr uint16_t MAX_TRACES = 512;

    static uint16_t trace_begin(const char *name);
    static void     trace_end(uint16_t slot);
    static void     dump_traces();
    static void     dump_task_stats();

private:
    Profiler() = delete;
    static TracePoint _points[MAX_TRACES];
    static uint16_t _count;
};

#define PROFILE_BEGIN(name) \
    uint16_t __profile_slot_##__LINE__ = Profiler::trace_begin(name)

#define PROFILE_END(name) \
    do { (void)name; Profiler::trace_end(__profile_slot_##__LINE__); } while (0)

class ProfileBlock
{
public:
    ProfileBlock(const char *name)
        : _slot(Profiler::trace_begin(name)), _name(name) {}
    ~ProfileBlock() { Profiler::trace_end(_slot); }
private:
    uint16_t _slot;
    const char *_name;
};

#define PROFILE_BLOCK(name) ProfileBlock __profile_block_##__LINE__(name)
