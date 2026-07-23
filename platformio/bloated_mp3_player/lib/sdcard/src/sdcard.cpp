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
* FILE: sdcard.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 12:7:50 23-07-2026
* DESCRIPTION:
* Talks to the microSD card over SPI. The card holds MP3s, WAVs, and
* the shattered remains of the user's expectation of good sound quality.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: SD card implementation.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/sdcard.hpp"

static bool mounted = false;
static SDCard::TrackInfo track_list[SDCard::MAX_TRACKS];
static size_t track_count = 0;
static const char *audio_ext[] = { ".wav", ".WAV", ".mp3", ".MP3" };

bool SDCard::begin(uint8_t clk, uint8_t cmd, uint8_t d0, bool format_if_fail)
{
    // The generic esp32s3 variant doesn't define BOARD_HAS_SDMMC,
    // so the SD_MMC constructor leaves _pin_* = -1 and begin()
    // fails.  We must set the pins explicitly.
    SD_MMC.setPins(clk, cmd, d0);

    if (!SD_MMC.begin("", 1, format_if_fail))  // 1-bit SDMMC
    {
        mounted = false;
        return false;
    }
    mounted = true;
    return true;
}

bool SDCard::is_mounted()
{
    return mounted;
}

File SDCard::open(const char *path)
{
    if (!mounted) {
        return File();
    }
    return SD_MMC.open(path, FILE_READ);
}

void SDCard::close(File &f)
{
    if (f) {
        f.close();
    }
}

size_t SDCard::read(File &f, uint8_t *buf, size_t len)
{
    if (!f) {
        return 0;
    }
    return f.read(buf, len);
}

bool SDCard::seek(File &f, uint32_t pos)
{
    if (!f) {
        return false;
    }
    return f.seek(pos);
}

uint32_t SDCard::position(File &f)
{
    if (!f) {
        return 0;
    }
    return f.position();
}

uint32_t SDCard::size(File &f)
{
    if (!f) {
        return 0;
    }
    return f.size();
}

bool SDCard::is_audio_file(const char *path)
{
    for (auto ext : audio_ext) {
        const char *p = strstr(path, ext);
        if (p && strlen(p) == strlen(ext)) {
            return true;
        }
    }
    return false;
}

static void scan_dir(const char *dir)
{
    if (track_count >= SDCard::MAX_TRACKS) {
        return;
    }
    File root = SD_MMC.open(dir);
    if (!root) {
        return;
    }

    size_t dlen = strlen(dir);
    bool dir_has_slash = (dlen > 0 && dir[dlen - 1] == '/');

    File file;
    while ((file = root.openNextFile()) && track_count < SDCard::MAX_TRACKS) {
        if (file.isDirectory()) {
            char sub[SDCard::MAX_SUBPATH_LEN];
            if (dir_has_slash) {
                snprintf(sub, sizeof(sub), "%s%s", dir, file.name());
            } else {
                snprintf(sub, sizeof(sub), "%s/%s", dir, file.name());
            }
            scan_dir(sub);
        } else if (SDCard::is_audio_file(file.name())) {
            SDCard::TrackInfo &ti = track_list[track_count];
            strncpy(ti.path, file.path(), sizeof(ti.path));
            strncpy(ti.filename, file.name(), sizeof(ti.filename));
            ti.size = file.size();
            ti.is_wav = strstr(file.name(), ".wav") || strstr(file.name(), ".WAV");
            track_count++;
        }
        file.close();
    }
    root.close();
}

bool SDCard::scan_tracks(const char *dir)
{
    if (!mounted) {
        return false;
    }
    track_count = 0;
    scan_dir(dir);
    return track_count > 0;
}

size_t SDCard::list_dir(const char *path, DirEntry *entries, size_t max_entries)
{
    if (!mounted || !entries || max_entries == 0) {
        return 0;
    }
    File dir = SD_MMC.open(path);
    if (!dir) {
        return 0;
    }

    size_t n = 0;
    File f;
    while ((f = dir.openNextFile()) && n < max_entries) {
        strncpy(entries[n].name, f.name(), sizeof(entries[n].name));
        entries[n].name[sizeof(entries[n].name) - 1] = '\0';
        entries[n].is_dir = f.isDirectory();
        n++;
        f.close();
    }
    dir.close();
    return n;
}

uint32_t SDCard::total_tracks()
{
    return track_count;
}

const SDCard::TrackInfo *SDCard::get_track(uint32_t index)
{
    if (index >= track_count) {
        return nullptr;
    }
    return &track_list[index];
}
