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
* FILE: tasks.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:13:9 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the headerfile that will contain the tasks that the esp is meant to perform.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include "serial.hpp"
#include "shared_instances.hpp"
namespace My
{
    namespace Tasks
    {
        // ─── UI Task ──────────────────────────────────────────────────────────
        void ui(void *pvParameters);

        // ─── Audio Task ───────────────────────────────────────────────────────
        void audio(void *pvParameters);

        // ─── Sensor Task ──────────────────────────────────────────────────────
        void sensor(void *pvParameters);

        // ─── LED Task ─────────────────────────────────────────────────────────
        void led(void *pvParameters);

        // ─── Matrix Task ─────────────────────────────────────────────────────────
        void matrix(void *pvParameters);

        // ─── Input Task ───────────────────────────────────────────────────────
        void input(void *pvParameters);

        // ─── Serial Task ───────────────────────────────────────────────────────
        void serial(void *pvParameters);
    } // namespace Tasks
} // namespace My
