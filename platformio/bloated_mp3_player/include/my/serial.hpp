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
    /**
     * @brief Queue-based UART output wrapper.
     *
     * All log output goes through a single FreeRTOS queue: every task
     * formats its message and enqueues it in one atomic xQueueSend call,
     * and a dedicated serial output task drains the queue in order.
     * This avoids UART contention between tasks and guarantees lines are
     * never interleaved.
     *
     * Stack usage note: serial_print()/serial_debug() format with
     * vsnprintf() (newlib printf engine, ~1.5KB of stack on the calling
     * task). Use serial_print_raw() on hot paths (per-tick output) where
     * the message is already formatted, to avoid that tax.
     */
    class Serial {
        public:
        /**
         * @brief Construct the serial wrapper.
         * @param serial_msg_len    Size of one queue item (max message length, bytes).
         * @param serial_queue_len  Number of messages the queue can hold.
         * @param uart_baud         Baud rate of the raw UART.
         */
        Serial(const uint32_t serial_msg_len = SERIAL_MSG_LEN, const uint32_t serial_queue_len = SERIAL_QUEUE_LEN, const unsigned long uart_baud = My::Config::UART_BAUD);
        ~Serial();

        /**
         * @brief Create the message queue and start the raw UART.
         * @note  Must be called once before any print function is used.
         */
        void initialise();

        /**
         * @brief Format a message (printf-style) and enqueue it.
         * @param fmt  printf-style format string.
         * @param ...  Format arguments.
         * @note  Costs ~1.5KB of stack on the calling task (vsnprintf).
         */
        void serial_print(const char *fmt, ...);

        /**
         * @brief Format and enqueue a message, gated by a debug flag.
         * @param display  Compile-time/runtime switch; if false the message is dropped.
         * @param fmt      printf-style format string.
         * @param ...      Format arguments.
         * @note  Compiled out entirely unless ENABLE_GLOBAL_DEBUGGING is defined.
         */
        void serial_debug(const bool display, const char *fmt, ...);

        /**
         * @brief Enqueue an already-formatted message without formatting.
         * @param msg  Null-terminated string to enqueue.
         * @note  Cheap (~200B of stack): no vsnprintf. Use on hot paths
         *        (e.g. profiler output) where the string is already built.
         */
        void serial_print_raw(const char *msg);

        /**
         * @brief Block until a queued message is available.
         * @param msg  Buffer (>= serial_msg_len bytes) receiving the message.
         */
        void get_queue(char *msg);

        /**
         * @brief Write one message straight to the UART (bypasses the queue).
         * @param msg  Null-terminated string to print.
         */
        void dump_to_uart(char *msg);

        private:
        const unsigned long _uart_baud;       ///< Baud rate of the raw UART.
        uint32_t _serial_msg_len;             ///< Size of one queue item (bytes).
        uint32_t _serial_queue_len;           ///< Capacity of the message queue.
        QueueHandle_t _serial_queue = nullptr; ///< FreeRTOS message queue.
        HardwareSerial _raw_serial;            ///< Direct UART used by the drain task.
    };
}
