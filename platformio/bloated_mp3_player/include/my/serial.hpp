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
* FILE: serial.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 12:0:56 23-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file containing the content in icharge of dispmaying text on the UART terminal.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once
#include "config.hpp"
#include <Arduino.h>
#include "config/debug.hpp"

namespace My
{
    class Serial {
        public:
        Serial(const uint32_t serial_msg_len = SERIAL_MSG_LEN, const uint32_t serial_queue_len = SERIAL_QUEUE_LEN, const unsigned long uart_baud = My::Config::UART_BAUD);
        ~Serial();
        void initialise();
        void serial_print(const char *fmt, ...);
        void serial_debug(const bool display, const char *fmt, ...);
        void get_queue(char *msg);
        void dump_to_uart(char *msg);
        private:
        const unsigned long _uart_baud;
        uint32_t _serial_msg_len;
        uint32_t _serial_queue_len;
        QueueHandle_t _serial_queue = nullptr;
        HardwareSerial _raw_serial;
    };
}
