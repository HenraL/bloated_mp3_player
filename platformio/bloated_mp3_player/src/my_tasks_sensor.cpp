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
* FILE: my_tasks_sensor.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 12:41:41 23-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the sensor section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <imu.hpp>
#include <audio.hpp>
#include <profiling.hpp>
#include "my/tasks.hpp"
#include "shared_instances.hpp"


namespace My
{
    namespace Tasks
    {
        // ─── Sensor Task ──────────────────────────────────────────────────────
        void sensor(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(200);
            SharedInstances::serial.serial_print("[Sensor] Time is an illusion. Lunchtime doubly so.");

            while (true) {
                PROFILE_BLOCK("sensor_tick");

                IMU::Vec3 accel;
                IMU::read_accel(accel);
                IMU::Gesture g = IMU::gesture_tick();

                switch (g) {
                    case IMU::Gesture::Shake:
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_ANGLE_SENSOR_SHAKE, "[Sensor] Shake");
                        // Audio::stop();
                        // Audio::play_raw(nullptr, 0);
                        break;
                    case IMU::Gesture::TiltLeft:
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_ANGLE_SENSOR_TILT_LEFT, "[Sensor] TiltLeft");
                        // Audio::pause();
                        break;
                    case IMU::Gesture::TiltRight:
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_ANGLE_SENSOR_TILT_RIGHT, "[Sensor] TiltRight");
                        // Audio::resume();
                        break;
                    default:
                        break;
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
