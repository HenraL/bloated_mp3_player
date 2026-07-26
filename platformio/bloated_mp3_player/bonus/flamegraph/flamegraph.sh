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
# LAST Modified: 0:0:0 26-07-2026
# DESCRIPTION:
# This is the code in charge of making the bloated player come to life.
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: This bash file wraps the flamegraph Python converter,
#          handling venv activation and passing arguments through.
# // AR
# +==== END Bloated MP3 Player =================+
#
# Generate interactive SVG flamegraphs from profiling serial dumps.
# Run from the bonus/ directory:
#   ./extras/flamegraph.sh -h
#   ./extras/flamegraph.sh < ../tmp/test_batch/profile_run_001.txt
#   ./extras/flamegraph.sh ../tmp/test_batch/profile_run_001.txt
#   ./extras/flamegraph.sh --scan ../tmp/

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ENV_NAME="$SCRIPT_DIR/../lenv"
ENV_PATH="$ENV_NAME/bin/activate"
PYTHON_SCRIPT="$SCRIPT_DIR/flamegraph.py"
REQUIREMENTS="$SCRIPT_DIR/requirements.txt"

function activate_env() {
    if [ -f "$ENV_PATH" ]; then
        . $ENV_PATH
    else
        echo "Virtual environment not found at $ENV_PATH"
        exit 1
    fi
}

function deactivate_env() {
    deactivate 2>/dev/null
}

function ensure_env() {
    if [ ! -d "$ENV_NAME" ]; then
        echo "Creating virtual environment..."
        python3 -m venv "$ENV_NAME"
        echo "Activating virtual environment..."
        source "$ENV_PATH"
        echo "Installing required packages..."
        pip install -r "$REQUIREMENTS"
        echo "Disabling virtual environment..."
        deactivate
    fi
}

function conversion_status() {
    local STATUS=$1
    if [ $STATUS -ne 0 ]; then
        echo "Error: Flamegraph generation failed."
        deactivate_env
        exit $STATUS
    fi
    echo "Flamegraph generation completed."
    deactivate_env
    exit $STATUS
}

ensure_env

if [ $# -ne 0 ]; then
    if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
        activate_env
        python "$PYTHON_SCRIPT" -h
        deactivate_env
        exit 0
    fi
    activate_env
    echo "Starting flamegraph generation..."
    python "$PYTHON_SCRIPT" $@
    conversion_status $?
else
    echo "Starting flamegraph generation..."
    echo "Reading profiling data from stdin (pipe or redirect)..."
    activate_env
    python "$PYTHON_SCRIPT"
    conversion_status $?
fi
