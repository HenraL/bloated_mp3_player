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
* FILE: sdcard.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* microSD card abstraction. Reads WAV files (and eventually MP3s if we
* can find a decoder that doesn't require more RAM than the known
* universe has atoms).
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: SD card file I/O for audio tracks.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include <SD_MMC.h>
#include <SPI.h>
#include "sdcard_constants.hpp"
#include "sdcard_structures.hpp"

namespace SDCard
{
    bool begin(uint8_t clk, uint8_t cmd, uint8_t d0, bool format_if_fail = false);
    bool is_mounted();
    File open(const char *path);
    void close(File &f);
    size_t read(File &f, uint8_t *buf, size_t len);
    bool seek(File &f, uint32_t pos);
    uint32_t position(File &f);
    uint32_t size(File &f);

    bool scan_tracks(const char *dir);
    bool is_audio_file(const char *path);
    uint32_t total_tracks();
    const TrackInfo *get_track(uint32_t index);

    size_t list_dir(const char *path, DirEntry *entries, size_t max_entries);
}
