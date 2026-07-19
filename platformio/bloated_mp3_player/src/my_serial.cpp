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
* FILE: my_serial.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:26:26 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the function with the logic in charge of displaying text on the terminal UART.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "my/serial.hpp"

namespace My
{

    Serial::Serial(const uint32_t serial_msg_len, const uint32_t serial_queue_len)
        : _serial_msg_len(serial_msg_len), _serial_queue_len(serial_queue_len)
    {
        _serial_queue = nullptr;
    }

    Serial::~Serial() {};

    void Serial::initialise()
    {
        _serial_queue = xQueueCreate(_serial_queue_len, _serial_msg_len);
    }

    void Serial::serial_print(const char *fmt, ...)
    {
        char buf[_serial_msg_len];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        xQueueSend(_serial_queue, buf, portMAX_DELAY);
    }

    void Serial::get_queue(char *msg)
    {
        xQueueReceive(_serial_queue, msg, portMAX_DELAY);
    }
}
