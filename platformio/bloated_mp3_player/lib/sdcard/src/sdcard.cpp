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
* LAST Modified: 20:39:12 15-07-2026
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
// Use SPI3_HOST (= 2) to keep SD on a separate peripheral from the LCD
// which uses the default SPI (SPI2_HOST = 1 on S3, 1 on ESP32).
static SPIClass sd_spi(2);
static SDCard::TrackInfo track_list[SDCard::MAX_TRACKS];
static size_t track_count = 0;
static const char *audio_ext[] = {".wav", ".mp3", ".WAV", ".MP3"};

bool SDCard::begin(uint8_t cs_pin, uint8_t mosi_pin,
                   uint8_t miso_pin, uint8_t sclk_pin)
{
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    delay(10);
    sd_spi.begin(sclk_pin, miso_pin, mosi_pin, cs_pin);
    if (!SD.begin(cs_pin, sd_spi, 1000000))
    {
        mounted = false;
        digitalWrite(cs_pin, HIGH);
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
    if (!mounted) return File();
    return SD.open(path, FILE_READ);
}

void SDCard::close(File &f)
{
    if (f) f.close();
}

size_t SDCard::read(File &f, uint8_t *buf, size_t len)
{
    if (!f) return 0;
    return f.read(buf, len);
}

bool SDCard::seek(File &f, uint32_t pos)
{
    if (!f) return false;
    return f.seek(pos);
}

uint32_t SDCard::position(File &f)
{
    if (!f) return 0;
    return f.position();
}

uint32_t SDCard::size(File &f)
{
    if (!f) return 0;
    return f.size();
}

bool SDCard::is_audio_file(const char *path)
{
    for (auto ext : audio_ext)
    {
        const char *p = strstr(path, ext);
        if (p && strlen(p) == strlen(ext))
            return true;
    }
    return false;
}

bool SDCard::scan_tracks(const char *dir)
{
    if (!mounted) return false;
    track_count = 0;
    File root = SD.open(dir);
    if (!root) return false;

    File file;
    while ((file = root.openNextFile()) && track_count < MAX_TRACKS)
    {
        if (!file.isDirectory())
        {
            if (is_audio_file(file.name()))
            {
                TrackInfo &ti = track_list[track_count];
                strncpy(ti.path, file.path(), sizeof(ti.path));
                strncpy(ti.filename, file.name(), sizeof(ti.filename));
                ti.size = file.size();
                ti.is_wav = strstr(file.name(), ".wav") ||
                            strstr(file.name(), ".WAV");
                track_count++;
            }
        }
        file.close();
    }
    root.close();
    return true;
}

uint32_t SDCard::total_tracks()
{
    return track_count;
}

const SDCard::TrackInfo *SDCard::get_track(uint32_t index)
{
    if (index >= track_count) return nullptr;
    return &track_list[index];
}
