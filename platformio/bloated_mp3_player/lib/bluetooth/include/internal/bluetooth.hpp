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
* LAST Modified: 11-08-2026
* DESCRIPTION:
* BLE UART bridge. Nothing says "bloated" like mirroring the debug feed
* over the air to a phone while a temperature sensor judges you from
* the I2C bus.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: BLE UART bridge (nRF UART service) for wireless debug output.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <BLEServer.h>

namespace Bluetooth
{
    enum class State : uint8_t
    {
        Idle,
        Connected,
        Disconnected,
        Error
    };

    /**
     * @brief BLE UART bridge (nRF UART service, 6E400001-...).
     *
     * The ESP32-S3 SDK ships no classic-Bluetooth SPP/GAP stack (the esp32
     * libbt.a has it, the esp32s3 one does not — verified with nm), so
     * BluetoothSerial can never link on this board. The debug feed is
     * instead mirrored over a BLE notify characteristic, readable from any
     * phone/PC "BLE serial terminal" app. Long lines are split into
     * MTU-sized chunks so nothing gets eaten by the attribute layer.
     */
    class Bridge
    {
    public:
        Bridge();
        bool begin(const char *device_name = "BloatedMP3");
        State get_state() const;
        bool is_connected() const;
        State wait_for_connection(uint32_t timeout_ms = 30000);
        bool uart_connected() const;
        void uart_stream(const char *line);
    private:
        // Largest ATT payload with a 247-byte MTU: 247 minus 3 bytes of
        // attribute header, minus the 3 bytes a notification costs.
        static const size_t TX_CHUNK_MAX = 241;
        static const uint16_t DEFAULT_MTU = 247;

        class ServerCallbacks : public BLEServerCallbacks
        {
        public:
            explicit ServerCallbacks(Bridge *bridge);
            void onConnect(BLEServer *server) override;
            void onDisconnect(BLEServer *server) override;
        private:
            Bridge *_bridge;
        };

        void set_state(State state);

        BLEUUID _service_uuid;
        BLEUUID _tx_uuid;
        BLEUUID _rx_uuid;
        BLEServer *_server;
        BLECharacteristic *_tx;
        BLECharacteristic *_rx;
        volatile bool _connected;
        State _state;
        ServerCallbacks _callbacks;
    };
}
