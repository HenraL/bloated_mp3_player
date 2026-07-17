""" 
# +==== BEGIN AsperHeader =================+
# LOGO:
# ..........####...####..........
# ......###.....#.#########......
# ....##........#.###########....
# ...#..........#.############...
# ...#..........#.#####.######...
# ..#.....##....#.###..#...####..
# .#.....#.##...#.##..##########.
# #.....##########....##...######
# #.....#...##..#.##..####.######
# .#...##....##.#.##..###..#####.
# ..#.##......#.#.####...######..
# ..#...........#.#############..
# ..#...........#.#############..
# ...##.........#.############...
# ......#.......#.#########......
# .......#......#.########.......
# .........#####...#####.........
# /STOP
# PROJECT: AsperHeader
# FILE: full_erase.py
# CREATION DATE: 17-07-2026
# LAST Modified: 13:39:4 17-07-2026
# DESCRIPTION:
# rr
# /STOP
# COPYRIGHT: (c) Asperguide
# PURPOSE: 
# // AR
# +==== END AsperHeader =================+
""" 
""" 
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
# FILE: full_erase.py
# CREATION DATE: 16-07-2026
# DESCRIPTION:
# Erase the ESP32 flash before flashing new firmware.
#
# Can be used as:
#   1) A PlatformIO extra_script (pre step, auto-detects port)
#   2) A standalone CLI tool
#
# Usage (standalone):
#   python full_erase.py                          # auto-detect port
#   python full_erase.py --port /dev/ttyUSB0      # explicit port
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: Erase ESP32 flash via esptool
# // AR
# +==== END Bloated MP3 Player =================+
""" 

import os
import sys
import shutil
import subprocess
import argparse
import typing


# ─── File encoding ───────────────────────────────────────────────────────
FILE_ENCODING: str = "utf-8"

# ─── Default serial port (fallback) ───────────────────────────────────────
DEFAULT_PORT: str = "/dev/ttyACM0"


def find_esptool() -> typing.Optional[str]:
    """Locate esptool.py in the system PATH.

    Returns the full path if found, None otherwise.
    """
    found: typing.Optional[str] = shutil.which("esptool.py")
    if found is not None:
        return found
    found = shutil.which("esptool")
    return found


def detect_port() -> typing.Optional[str]:
    """Try to auto-detect the ESP32-S3 port via platformio device list.

    Returns the port path or None if detection fails.
    """
    try:
        result: subprocess.CompletedProcess = subprocess.run(
            ["platformio", "device", "list"],
            capture_output=True,
            text=True,
            timeout=10,
            check=False
        )
        if result.returncode != 0:
            return None
        for line in result.stdout.splitlines():
            if "ACM" in line or "USB" in line:
                parts: typing.List[str] = line.strip().split()
                if parts:
                    return parts[0]
        return None
    except (subprocess.TimeoutExpired, FileNotFoundError, OSError):
        return None


def erase_flash(port: str, esptool_path: str) -> int:
    """Run esptool to erase the flash on the given port.

    Returns the esptool exit code.
    """
    print(f"Erasing flash on {port} using {esptool_path}...")
    result: int = subprocess.call(
        [esptool_path, "--port", port, "erase-flash"]
    )
    if result == 0:
        print("Flash erase completed successfully.")
    else:
        print(f"Flash erase failed with exit code {result}.", file=sys.stderr)
    return result


def main() -> None:
    """Standalone entry point: parse arguments and erase flash."""
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Erase ESP32 flash via esptool"
    )
    parser.add_argument(
        "--port", default=None,
        help="Serial port (auto-detected if omitted)"
    )
    args: argparse.Namespace = parser.parse_args()

    esptool_path: typing.Optional[str] = find_esptool()
    if esptool_path is None:
        print(
            "Error: esptool.py not found in PATH. "
            "Install it via: pip install esptool",
            file=sys.stderr,
        )
        sys.exit(1)

    port: typing.Optional[str] = args.port
    if port is None:
        port = detect_port()
    if port is None:
        port = DEFAULT_PORT
        print(
            f"Could not auto-detect port, falling back to {port}",
            file=sys.stderr,
        )

    exit_code: int = erase_flash(port, esptool_path)
    sys.exit(exit_code)


if __name__ == "__main__":
    main()
