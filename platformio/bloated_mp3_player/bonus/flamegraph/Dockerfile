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
# FILE: Dockerfile
# CREATION DATE: 26-07-2026
# LAST Modified: 30-07-2026
# DESCRIPTION:
# This is the code in charge of making the bloated player come to life.
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: This is the docker container in charge of deploying the flamegraph generation software.
# // AR
# +==== END Bloated MP3 Player =================+
# 
FROM perl:5.44.0-bookworm

LABEL description="FlameGraph tools — Brendan Gregg's flamegraph.pl for Bloated MP3 Player"
LABEL source="https://github.com/brendangregg/FlameGraph"

# Install wget to grab flamegraph.pl from GitHub
RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends wget ca-certificates; \
    rm -rf /var/lib/apt/lists/*; \
    wget -O /usr/local/bin/flamegraph.pl \
        https://raw.githubusercontent.com/brendangregg/FlameGraph/master/flamegraph.pl; \
    chmod +x /usr/local/bin/flamegraph.pl

# Install the profiling filter
COPY profiler_filter.pl /usr/local/bin/profiler_filter.pl
RUN chmod +x /usr/local/bin/profiler_filter.pl

# Wrapper entrypoint: pipe through filter then flamegraph.pl
COPY flamegraph_wrapper.sh /usr/local/bin/flamegraph_wrapper.sh
RUN chmod +x /usr/local/bin/flamegraph_wrapper.sh

ENTRYPOINT ["/usr/local/bin/flamegraph_wrapper.sh"]
CMD ["--help"]
