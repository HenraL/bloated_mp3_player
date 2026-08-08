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
static SDCard::FolderInfo folder_list[SDCard::MAX_FOLDERS];
static size_t folder_count = 0;
static const char *audio_ext[] = { ".wav", ".WAV", ".mp3", ".MP3" };

bool SDCard::begin(uint8_t clk, uint8_t cmd, uint8_t d0, bool format_if_fail)
{
    // The generic esp32s3 variant doesn't define BOARD_HAS_SDMMC,
    // so the SD_MMC constructor leaves _pin_* = -1 and begin()
    // fails.  We must set the pins explicitly.
    SD_MMC.setPins(clk, cmd, d0);

    if (!SD_MMC.begin("", 1, format_if_fail, SDMMC_FREQ_DEFAULT))  // 1-bit SDMMC @ 20 MHz
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

bool SDCard::is_printable(const char *str, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (str[i] < 0x20 || str[i] > 0x7E) {
            return false;
        }
    }
    return true;
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
            const char *full_path = file.path();
            const char *fname = file.name();
            size_t path_len = strlen(full_path);
            size_t fname_len = strlen(fname);

            // Over-long or non-printable (corrupt) names: skip, never truncate.
            // A truncated path without a NUL terminator leaks adjacent memory
            // into every open() call and shows up as garbage in Serial/logs.
            if (path_len >= SDCard::TRACK_PATH_LEN
                || fname_len >= SDCard::TRACK_FILENAME_LEN
                || !SDCard::is_printable(full_path, path_len)) {
                continue;
            }
            SDCard::TrackInfo &ti = track_list[track_count];
            strncpy(ti.path, full_path, sizeof(ti.path));
            ti.path[sizeof(ti.path) - 1] = '\0';
            strncpy(ti.filename, fname, sizeof(ti.filename));
            ti.filename[sizeof(ti.filename) - 1] = '\0';

            // Keep the parent folder so the UI can group tracks by album.
            {
                const char *slash = strrchr(ti.path, '/');
                size_t folder_len = slash ? (size_t)(slash - ti.path) : 0;
                if (folder_len > 0 && folder_len < sizeof(ti.folder)) {
                    strncpy(ti.folder, ti.path, folder_len);
                    ti.folder[folder_len] = '\0';
                } else {
                    ti.folder[0] = '\0';
                }
            }
            ti.size = file.size();
            ti.is_wav = strstr(file.name(), ".wav") || strstr(file.name(), ".WAV");
            track_count++;

            // Register (or extend) the folder this track lives in, keeping
            // tracks grouped by folder so the browser can walk folder order.
            {
                bool found = false;
                for (size_t f = 0; f < folder_count; f++) {
                    if (strcmp(folder_list[f].folder, ti.folder) == 0) {
                        folder_list[f].track_count++;
                        found = true;
                        break;
                    }
                }
                if (!found && folder_count < SDCard::MAX_FOLDERS) {
                    SDCard::FolderInfo &fi = folder_list[folder_count];
                    strncpy(fi.folder, ti.folder, sizeof(fi.folder));
                    fi.folder[sizeof(fi.folder) - 1] = '\0';
                    fi.first_track = (uint32_t)(track_count - 1);
                    fi.track_count = 1;
                    folder_count++;
                }
            }
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
        folder_count = 0;
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

uint32_t SDCard::total_folders()
{
    return folder_count;
}

const SDCard::FolderInfo *SDCard::get_folder(uint32_t index)
{
    if (index >= folder_count) {
        return nullptr;
    }
    return &folder_list[index];
}
