""" 
# +==== BEGIN LCD Screen =================+
# LOGO:
# ......................
# ...________________...
# ..|.>_.............|..
# ..|................|..
# ..|................|..
# ..|________________|..
# ......................
# /STOP
# PROJECT: LCD Screen
# FILE: full_erase.py
# CREATION DATE: 22-05-2026
# LAST Modified: 19:54:53 07-07-2026
# DESCRIPTION:
# This is code in charge of driving an lcd screen.
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: This is the script that will erase the flash of the ESP32. It is used to reset the device to factory settings before flashing a new firmware. It uses the esptool utility to perform the erase operation.
# // AR
# +==== END LCD Screen =================+
""" 

from subprocess import call

call(
    [
        "/home/hanra/manually_installed_software/shortcuts/esptool",
        "--port", 
        "/dev/ttyACM0", 
        "erase-flash"
        ]
    )
