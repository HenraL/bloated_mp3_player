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
* FILE: bluetooth.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Bluetooth A2DP audio sink. Pairs with your wireless headphones so
* they too can experience the glorious low-bitrate warmth of the
* ESP32-S3's I2S output.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Bluetooth A2DP sink for wireless audio.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>

namespace Bluetooth
{
    enum class State : uint8_t
    {
        Idle,
        Pairing,
        Connected,
        Disconnected,
        Error
    };

    bool begin(const char *device_name = "BloatedMP3");
    State get_state();
    bool is_connected();
    void disconnect();
    void start_scan();
    bool pair(const char *address);
    void tick();
    State wait_for_connection(uint32_t timeout_ms = 30000);
}
