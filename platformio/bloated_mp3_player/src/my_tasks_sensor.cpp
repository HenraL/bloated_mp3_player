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
* LAST Modified: 21:23:4 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the sensor section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <environmental.hpp>
#include <imu.hpp>
#include <audio.hpp>
#include <profiling.hpp>
#include "my/tasks.hpp"
#include "shared_instances.hpp"

namespace MyTasks
{
    // ─── Sensor Task ──────────────────────────────────────────────────────
    void sensor(void *pvParameters)
    {
        (void)pvParameters;
        TickType_t xLastWake = xTaskGetTickCount();
        const TickType_t freq = pdMS_TO_TICKS(200);

        while (true) {
            SharedInstances::serial.serial_print("[Sensor] Time is an illusion. Lunchtime doubly so.");
            PROFILE_BLOCK("sensor_tick");
            Environmental::Reading env;
            Environmental::read(env);

            IMU::Vec3 accel;
            IMU::read_accel(accel);
            IMU::Gesture g = IMU::gesture_tick();

            switch (g) {
                case IMU::Gesture::Shake:
                    Audio::stop();
                    Audio::play_raw(nullptr, 0);
                    break;
                case IMU::Gesture::TiltLeft:
                    Audio::pause();
                    break;
                case IMU::Gesture::TiltRight:
                    Audio::resume();
                    break;
                default: break;
            }

            vTaskDelayUntil(&xLastWake, freq);
        }
    }
} // namespace MyTasks
