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
# FILE: font2c.sh
# CREATION DATE: 23-07-2026
# LAST Modified: 0:55:11 23-07-2026
# DESCRIPTION:
# This is the code in charge of making the bloated player come to life.
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: This is the bash file in charge of helping an easy setup for the convertion of images (or whatever image files that are supported by the converter) to a c file (or library)
# // AR
# +==== END Bloated MP3 Player =================+
# 
# Convert a font to a C header file.
ENV_NAME="img2c_env"
ENV_PATH="$(pwd)/$ENV_NAME/bin/activate"
PYTHON_SCRIPT="./img2c/convert.py"

function activate_env() {
    echo "Activating virtual environment..."
    if [ -f "$ENV_PATH" ]; then
        . $ENV_PATH
    else
        echo "Virtual environment not found. Please run the script again to create it."
        exit 1
    fi
}

function deactivate_env() {
    echo "Deactivating virtual environment..."
    deactivate
}

function ensure_env() {
    if [ ! -d "$ENV_NAME" ]; then
        echo "Creating virtual environment..."
        python3 -m venv $ENV_NAME
        echo "Activating virtual environment..."
        source $ENV_NAME/bin/activate
        echo "Installing required packages..."
        pip install -r ./img2c/requirements.txt
        echo "Disabling virtual environment..."
        deactivate
    fi
}

function conversion_status() {
    local STATUS=$1
    if [ $STATUS -ne 0 ]; then
        echo "Error: Image conversion failed."
        deactivate_env
        exit $STATUS
    fi
    echo "Conversion completed."
    deactivate_env
    exit $STATUS
}

ensure_env

if [$# -ne 0 ]; then
    if [ '$1' == '-h' ]; then
        echo "Usage: ./img2c.sh --help"
        echo "Converts all fonts in the directory of your choice to C header files in the destination directory of your choice."
        echo "Underlying pythons script's help:"
        activate_env
        python $PYTHON_SCRIPT --help
        deactivate_env
        exit 0
    fi
    activate_env
    echo "Starting image conversion..."
    python $PYTHON_SCRIPT $@
    conversion_status $?
    
else
    activate_env
    echo "Starting font conversion..."
    python $PYTHON_SCRIPT --library --batch --output ../lib/images/ ./images/
    conversion_status $?
fi
