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
* FILE: bluetooth.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Bluetooth A2DP implementation. Because nothing says "bloated" like
* streaming terrible-quality audio wirelessly to a pair of overpriced
* earbuds while a temperature sensor judges you from the I2C bus.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Bluetooth A2DP sink.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/bluetooth.hpp"

static Bluetooth::State bt_state = Bluetooth::State::Idle;

bool Bluetooth::begin(const char *device_name)
{
    (void)device_name;
    bt_state = State::Idle;
    return true;
}

Bluetooth::State Bluetooth::get_state()
{
    return bt_state;
}

bool Bluetooth::is_connected()
{
    return bt_state == State::Connected;
}

void Bluetooth::disconnect()
{
    bt_state = State::Disconnected;
}

void Bluetooth::start_scan()
{
    bt_state = State::Pairing;
}

bool Bluetooth::pair(const char *address)
{
    (void)address;
    bt_state = State::Connected;
    return true;
}

void Bluetooth::tick()
{
}

Bluetooth::State Bluetooth::wait_for_connection(uint32_t timeout_ms)
{
    (void)timeout_ms;
    bt_state = State::Connected;
    return bt_state;
}
