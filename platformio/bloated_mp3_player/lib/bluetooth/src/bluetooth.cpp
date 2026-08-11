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
* LAST Modified: 11-08-2026
* DESCRIPTION:
* BLE UART bridge implementation. Advertises the nRF UART service
* (6E400001-...) and mirrors the serial debug feed to any connected
* phone/PC "BLE serial terminal" app via the TX notify characteristic.
* The RX characteristic exists so apps can attach to the service; it is
* the future home of phone-to-player commands.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: BLE UART bridge (nRF UART service) for wireless debug output.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "internal/bluetooth.hpp"
#include <BLEDevice.h>

Bluetooth::Bridge::Bridge() :
    _service_uuid(BLEUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e")),
    _tx_uuid(BLEUUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e")),
    _rx_uuid(BLEUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e")),
    _server(nullptr),
    _tx(nullptr),
    _rx(nullptr),
    _connected(false),
    _state(State::Idle),
    _callbacks(this)
{
}

bool Bluetooth::Bridge::begin(const char *device_name)
{
    if (device_name == nullptr) {
        _state = State::Error;
        return false;
    }
    BLEDevice::init(device_name);
    BLEDevice::setMTU(DEFAULT_MTU);
    _server = BLEDevice::createServer();
    if (_server == nullptr) {
        _state = State::Error;
        return false;
    }
    _server->setCallbacks(&_callbacks);
    BLEService *service = _server->createService(_service_uuid);
    if (service == nullptr) {
        _state = State::Error;
        return false;
    }
    _tx = service->createCharacteristic(_tx_uuid, BLECharacteristic::PROPERTY_NOTIFY);
    _rx = service->createCharacteristic(_rx_uuid, BLECharacteristic::PROPERTY_WRITE);
    service->start();
    BLEDevice::startAdvertising();
    _state = State::Disconnected;
    return true;
}

Bluetooth::State Bluetooth::Bridge::get_state() const
{
    return _state;
}

bool Bluetooth::Bridge::is_connected() const
{
    return _connected;
}

bool Bluetooth::Bridge::uart_connected() const
{
    return _connected;
}

Bluetooth::State Bluetooth::Bridge::wait_for_connection(uint32_t timeout_ms)
{
    const uint32_t deadline = millis() + timeout_ms;
    while (!_connected && millis() < deadline) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    _state = _connected ? State::Connected : State::Disconnected;
    return _state;
}

void Bluetooth::Bridge::uart_stream(const char *line)
{
    if (line == nullptr || _tx == nullptr || !_connected) {
        return;
    }
    const size_t len = strlen(line);
    size_t offset = 0;
    while (offset < len) {
        const size_t chunk_len = ((len - offset) > TX_CHUNK_MAX) ? TX_CHUNK_MAX : (len - offset);
        _tx->setValue(reinterpret_cast<uint8_t *>(const_cast<char *>(line + offset)), chunk_len);
        _tx->notify();
        offset += chunk_len;
        if (offset < len) {
            vTaskDelay(pdMS_TO_TICKS(2));
        }
    }
    const char newline[] = "\n";
    _tx->setValue(reinterpret_cast<uint8_t *>(const_cast<char *>(newline)), 1);
    _tx->notify();
}

void Bluetooth::Bridge::set_state(State state)
{
    _state = state;
}

Bluetooth::Bridge::ServerCallbacks::ServerCallbacks(Bridge *bridge) :
    _bridge(bridge)
{
}

void Bluetooth::Bridge::ServerCallbacks::onConnect(BLEServer *server)
{
    (void)server;
    if (_bridge != nullptr) {
        _bridge->_connected = true;
        _bridge->set_state(State::Connected);
    }
}

void Bluetooth::Bridge::ServerCallbacks::onDisconnect(BLEServer *server)
{
    (void)server;
    if (_bridge != nullptr) {
        _bridge->_connected = false;
        _bridge->set_state(State::Disconnected);
    }
    // The phone dropped us. Restart advertising so it can re-pair without
    // rebooting the player.
    BLEDevice::startAdvertising();
}
