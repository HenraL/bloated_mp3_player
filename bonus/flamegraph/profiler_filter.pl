#!/usr/bin/perl
#
# +==== BEGIN Bloated MP3 Player =================+
# LOGO:
# .......................
# ...><>.............<><.
# ..><>.><>.......<><.<><
# .><>.<><.><>.<><.<><.<>
# ..><>.><>.......<><.<><
# ...><>.............<><.
# .......................
# /STOP
# PROJECT: Bloated MP3 Player
# FILE: profiler_filter.pl
# CREATION DATE: 30-07-2026
# LAST Modified: 30-07-2026
# DESCRIPTION:
# Reads mixed serial output (human debug + PROFILING lines),
# extracts PROFILING: lines, accumulates durations per call path,
# and outputs folded format for flamegraph.pl.
#
# Input format:
#   PROFILING: <semicolon-separated-path> <duration_us>
#
# Output format (folded):
#   <path> <total_duration_us>
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: Filter + accumulator for flamegraph pipeline.
# // AR
# +==== END Bloated MP3 Player =================+
#
use strict;
use warnings;

my %counts;

while (my $line = <STDIN>)
{
    chomp $line;
    if ($line =~ /^PROFILING: \s* (.+?) \s+ (\d+) $/x)
    {
        my $path = $1;
        my $dur  = $2;
        $counts{$path} += $dur;
    }
}

for my $path (keys %counts)
{
    print "$path $counts{$path}\n";
}
