#!/bin/sh
#
# Wrapper entrypoint for the flamegraph container.
# Pipes stdin through the profiler filter (extracts PROFILING: lines,
# accumulates to folded format), then renders SVG via flamegraph.pl.
#
perl /usr/local/bin/profiler_filter.pl \
    | /usr/local/bin/flamegraph.pl "$@"
