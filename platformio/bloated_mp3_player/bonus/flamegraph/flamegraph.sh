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
# LAST Modified: 18:37:1 30-07-2026
# DESCRIPTION:
#   Docker-orchestrated flamegraph generator.
#   Pipes raw serial output (mixed debug + PROFILING lines) through a
#   Docker container that runs the profiler_filter.pl → flamegraph.pl
#   pipeline to produce an SVG.
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE:
#   Entry point for generating flamegraphs from embedded profiling data.
#   Uses Docker to run flamegraph.pl so no Perl installation needed
#   on the host — only Docker.
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
DOCKER_CMD="docker"

show_help() {
    echo "flamegraph.sh — Docker-orchestrated flamegraph generator"
    echo ""
    echo "USAGE"
    echo "  ./flamegraph.sh -h                          this help"
    echo "  ./flamegraph.sh < serial_dump.txt           SVG → stdout"
    echo "  ./flamegraph.sh dump.txt -o out.svg         SVG → named file"
    echo ""
    echo "MODES"
    echo "  stdin / file       Pipe raw serial output (human debug logs +"
    echo "                     PROFILING: lines) through the container."
    echo "                     The container filters and renders to SVG."
    echo ""
    echo "  --scan <dir>...    (fallback) Scan .cpp/.hpp files for"
    echo "                     PROFILE_BLOCK usage. Requires Python 3."
    echo ""
    echo "DEPENDENCIES"
    echo "  Docker   — required (runs Brendan Gregg's flamegraph.pl)"
    echo ""
    echo "EXAMPLES"
    echo "  minicom -D /dev/ttyACM0 -C dump.txt     # capture serial first"
    echo "  ./flamegraph.sh dump.txt -o profile.svg"
    echo "  screen /dev/ttyUSB0 115200 | tee dump.txt | ./flamegraph.sh > live.svg"
}

check_docker() {
    if ! command -v docker &>/dev/null; then
        echerr "Error: Docker is not installed or not in PATH."
        echerr "Please install Docker: https://docs.docker.com/get-docker/"
        exit 1
    fi

    echerr "Checking Docker accessibility..."
    if docker info &>/dev/null; then
        echerr "Docker accessible directly."
        DOCKER_CMD="docker"
    elif command -v sudo &>/dev/null; then
        echerr "Direct Docker access failed — trying with sudo..."
        if sudo docker info &>/dev/null; then
            echerr "Docker accessible via sudo."
            DOCKER_CMD="sudo docker"
        else
            echerr "Error: Docker daemon is not running or not accessible."
            echerr ""
            echerr "Tried both direct and sudo access. Common fixes:"
            echerr "  1. Add your user to the docker group and re-login:"
            echerr "       sudo usermod -aG docker \$USER"
            echerr "     then log out and back in."
            echerr "  2. Start the Docker daemon:"
            echerr "       sudo systemctl start docker"
            echerr "  3. Check your Docker context:"
            echerr "       docker context ls"
            echerr "       docker context use default"
            exit 1
        fi
    else
        echerr "Error: Docker daemon is not running or not accessible."
        echerr ""
        echerr "Direct access failed and sudo is not available."
        echerr "Add your user to the docker group:"
        echerr "  sudo usermod -aG docker \$USER"
        echerr "Then log out and back in."
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
    echerr "Checking for Docker image '$IMAGE_TAG'..."
    if [ ! -f "$DOCKERFILE" ]; then
        echerr "Error: Dockerfile not found at: $DOCKERFILE"
        exit 1
    fi

    LOCAL_HASH=$(md5sum "$DOCKERFILE" | cut -d' ' -f1)
    IMAGE_HASH=$($DOCKER_CMD image inspect "$IMAGE_TAG" \
        --format '{{index .Config.Labels "dockerfile.hash"}}' 2>/dev/null || echo "")

    if [ "$LOCAL_HASH" != "$IMAGE_HASH" ]; then
        if [ -n "$IMAGE_HASH" ]; then
            echerr "Dockerfile changed — rebuilding image."
        else
            echerr "Image not found — building from $DOCKERFILE ..."
        fi
        $DOCKER_CMD build -t "$IMAGE_TAG" \
            --label "dockerfile.hash=$LOCAL_HASH" \
            -f "$DOCKERFILE" "$SCRIPT_DIR"
        echerr "Done building image."
    else
        echerr "Image is up to date."
    fi
}

run_container() {
    $DOCKER_CMD run --rm -i "$IMAGE_TAG" "$@"
}

main() {
    for arg in "$@"; do
        if [ "$arg" = "-h" ] || [ "$arg" = "--help" ]; then
            show_help
            exit 0
        fi
    done

    if [ $# -eq 0 ] && [ -t 0 ]; then
        show_help
        exit 0
    fi

    check_docker
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

        check_python
        echerr "WARNING: --scan is a static fallback. Prefer runtime profiling."
        echerr "Scanning source directories: ${SCAN_DIRS[*]}"

        if [ -n "$OUTFILE" ]; then
            python3 "$PYTHON_SCRIPT" --scan "${SCAN_DIRS[@]}" \
                | run_container --title="Source Profile — ${SCAN_DIRS[*]}" \
                    --countname="blocks" \
                > "$OUTFILE"
            echerr "Wrote SVG output to: $OUTFILE"
        else
            python3 "$PYTHON_SCRIPT" --scan "${SCAN_DIRS[@]}" \
                | run_container --title="Source Profile — ${SCAN_DIRS[*]}" \
                    --countname="blocks"
        fi
        exit $?
    fi

    if [ -n "$INFILE" ]; then
        echerr "Processing input file: $INFILE"
        echerr "Running pipeline: raw serial → filter → flamegraph.pl → SVG"
        if [ -n "$OUTFILE" ]; then
            run_container < "$INFILE" > "$OUTFILE"
            echerr "Wrote SVG output to: $OUTFILE"
        else
            run_container < "$INFILE"
        fi
    else
        echerr "Reading from stdin..."
        echerr "Running pipeline: raw serial → filter → flamegraph.pl → SVG"
        if [ -n "$OUTFILE" ]; then
            run_container > "$OUTFILE"
            echerr "Wrote SVG output to: $OUTFILE"
        else
            run_container
        fi
    fi
}

main "$@"
