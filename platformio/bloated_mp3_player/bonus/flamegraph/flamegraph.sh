#!/usr/bin/env bash
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
# FILE: flamegraph.sh
# CREATION DATE: 26-07-2026
# LAST Modified: 18:40:18 30-07-2026
# DESCRIPTION:
#   Docker-orchestrated flamegraph generator.
#   Parses C++ source or serial dumps via flamegraph.py (outside Docker),
#   then pipes folded-format data into Brendan Gregg's flamegraph.pl
#   running inside a Docker container for SVG rendering.
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE:
#   Entry point for generating flamegraphs from C++ profiling data.
#   Uses Docker to run flamegraph.pl so no Perl installation needed
#   on the host — only Docker and Python 3 (stdlib).
# // AR
# +==== END Bloated MP3 Player =================+

set -euo pipefail

# stderr helper
echerr() { echo "$1" >&2; }

# Resolve paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_SCRIPT="$SCRIPT_DIR/flamegraph.py"
DOCKERFILE="$SCRIPT_DIR/Dockerfile"
IMAGE_TAG="flamegraph-tool"

show_help() {
    echo "flamegraph.sh — Docker-orchestrated flamegraph generator for C++ profiling"
    echo ""
    echo "USAGE"
    echo "  ./flamegraph.sh -h                          this help"
    echo "  ./flamegraph.sh --scan <dir>...             folded → stdout"
    echo "  ./flamegraph.sh --scan <dir>... > out.svg   SVG → file"
    echo "  ./flamegraph.sh < serial_dump.txt           SVG → stdout"
    echo "  ./flamegraph.sh dump.txt -o out.svg         SVG → named file"
    echo ""
    echo "MODES"
    echo "  --scan <dir> ...   Scan .cpp/.hpp files in one or more directories"
    echo "                     for PROFILE_BLOCK macros. Parsed by flamegraph.py"
    echo "                     on the host, rendered to SVG via Docker + flamegraph.pl."
    echo "  stdin / file       Read PROFILING sections from serial dump text"
    echo "                     and render to SVG."
    echo ""
    echo "DEPENDENCIES"
    echo "  Docker   — required (runs Brendan Gregg's flamegraph.pl)"
    echo "  Python 3 — required (stdlib only, for C++ data parsing)"
    echo ""
    echo "EXAMPLES"
    echo "  ./flamegraph.sh --scan src/ include/ > profile.svg"
    echo "  ./flamegraph.sh --scan src/ include/ lib/ -o profile.svg"
    echo "  ./flamegraph.sh < serial_dump.txt > trace.svg"
    echo "  ./flamegraph.sh dump.txt -o dump.svg"
}

check_docker() {
    if ! command -v docker &>/dev/null; then
        echerr "Error: Docker is not installed or not in PATH."
        echerr "Please install Docker: https://docs.docker.com/get-docker/"
        echerr ""
        echerr "Alternatively, you can use flamegraph.py directly for"
        echerr "folded-format output (without SVG rendering):"
        echerr "  python3 $PYTHON_SCRIPT --scan . > folded.txt"
        exit 1
    fi
    if ! docker info &>/dev/null; then
        echerr "Error: Docker daemon is not running or not accessible."
        echerr ""
        echerr "Common fixes:"
        echerr "  1. Add your user to the docker group and re-login:"
        echerr "       sudo usermod -aG docker \$USER"
        echerr "     then log out and back in."
        echerr "  2. Start the Docker daemon:"
        echerr "       sudo systemctl start docker"
        echerr "  3. Check your Docker context:"
        echerr "       docker context ls"
        echerr "       docker context use default"
        echerr ""
        echerr "Or use flamegraph.py directly for folded-format output."
        exit 1
    fi
}

check_python() {
    if ! command -v python3 &>/dev/null; then
        echerr "Error: python3 is not installed or not in PATH."
        exit 1
    fi
    if [ ! -f "$PYTHON_SCRIPT" ]; then
        echerr "Error: flamegraph.py not found at: $PYTHON_SCRIPT"
        exit 1
    fi
}

ensure_image() {
    if ! docker image inspect "$IMAGE_TAG" &>/dev/null; then
        echerr "Building Docker image '$IMAGE_TAG' from $DOCKERFILE ..."
        if [ ! -f "$DOCKERFILE" ]; then
            echerr "Error: Dockerfile not found at: $DOCKERFILE"
            exit 1
        fi
        docker build -t "$IMAGE_TAG" -f "$DOCKERFILE" "$SCRIPT_DIR"
        echerr "Done building image."
    fi
}

run_flamegraph_pl() {
    docker run --rm -i "$IMAGE_TAG" "$@"
}

main() {
    if [ $# -eq 0 ]; then
        show_help
        exit 0
    fi

    for arg in "$@"; do
        if [ "$arg" = "-h" ] || [ "$arg" = "--help" ]; then
            show_help
            exit 0
        fi
    done

    check_docker
    check_python
    ensure_image

    SCAN_MODE=false
    SCAN_DIRS=()
    OUTFILE=""
    INFILE=""

    i=1
    while [ $i -le $# ]; do
        arg="${!i}"
        case "$arg" in
            --scan)
                SCAN_MODE=true
                i=$((i + 1))
                while [ $i -le $# ]; do
                    nxt="${!i}"
                    if [[ "$nxt" == -* ]]; then
                        break
                    fi
                    SCAN_DIRS+=("$nxt")
                    i=$((i + 1))
                done
                ;;
            -o)
                i=$((i + 1))
                if [ $i -le $# ]; then
                    OUTFILE="${!i}"
                    i=$((i + 1))
                else
                    echerr "Error: -o requires a filename argument."
                    exit 1
                fi
                ;;
            --svg)
                i=$((i + 1))
                ;;
            -*)
                echerr "Error: unknown flag: $arg"
                exit 1
                ;;
            *)
                INFILE="$arg"
                i=$((i + 1))
                ;;
        esac
    done

    if [ "$SCAN_MODE" = true ]; then
        if [ ${#SCAN_DIRS[@]} -eq 0 ]; then
            SCAN_DIRS=(".")
        fi

        echerr "Scanning: ${SCAN_DIRS[*]}"

        if [ -n "$OUTFILE" ]; then
            python3 "$PYTHON_SCRIPT" --scan "${SCAN_DIRS[@]}" \
                | run_flamegraph_pl --title="Source Profile — ${SCAN_DIRS[*]}" \
                    --countname="blocks" \
                > "$OUTFILE"
            echerr "Wrote $OUTFILE"
        else
            python3 "$PYTHON_SCRIPT" --scan "${SCAN_DIRS[@]}" \
                | run_flamegraph_pl --title="Source Profile — ${SCAN_DIRS[*]}" \
                    --countname="blocks"
        fi
        exit $?
    fi

    if [ -n "$INFILE" ]; then
        if [ -n "$OUTFILE" ]; then
            python3 "$PYTHON_SCRIPT" "$INFILE" \
                | run_flamegraph_pl \
                > "$OUTFILE"
            echerr "Wrote $OUTFILE"
        else
            python3 "$PYTHON_SCRIPT" "$INFILE" \
                | run_flamegraph_pl
        fi
    else
        if [ -n "$OUTFILE" ]; then
            python3 "$PYTHON_SCRIPT" \
                | run_flamegraph_pl \
                > "$OUTFILE"
            echerr "Wrote $OUTFILE"
        else
            python3 "$PYTHON_SCRIPT" \
                | run_flamegraph_pl
        fi
    fi
}

main "$@"
