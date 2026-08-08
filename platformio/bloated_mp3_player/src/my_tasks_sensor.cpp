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
#include "my/infos.hpp"
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
            uint8_t orientation_log_countdown = 0;
            IMU::Vec3 accel;
            IMU::Gesture g;
            SharedInstances::serial.serial_print(My::Infos::sensor_time_illusion);

            while (true) {
                PROFILE_BLOCK("sensor_tick");

                if (SharedInstances::i2c_bus_lock != nullptr) {
                    xSemaphoreTake(SharedInstances::i2c_bus_lock, portMAX_DELAY);
                }
                IMU::read_accel(accel);
                g = IMU::gesture_tick();
                if (SharedInstances::i2c_bus_lock != nullptr) {
                    xSemaphoreGive(SharedInstances::i2c_bus_lock);
                }

                // Publish the latest orientation for the Vogon panel and log
                // it to UART on a sensible cadence (once per second, the
                // task ticks every 200ms).
                SharedInstances::imu_orientation = IMU::compute_orientation(accel);
                if (orientation_log_countdown == 0) {
                    orientation_log_countdown = 5;
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_ANGLE_SENSOR_ORIENTATION,
                        My::Infos::sensor_orientation,
                        SharedInstances::imu_orientation.roll,
                        SharedInstances::imu_orientation.pitch,
                        SharedInstances::imu_orientation.yaw
                    );
                } else {
                    orientation_log_countdown--;
                }

                switch (g) {
                    case IMU::Gesture::Shake:
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_ANGLE_SENSOR_SHAKE, My::Infos::sensor_shake);
                        // Audio::stop();
                        // Audio::play_raw(nullptr, 0);
                        break;
                    case IMU::Gesture::TiltLeft:
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_ANGLE_SENSOR_TILT_LEFT, My::Infos::sensor_tilt_left);
                        // Audio::pause();
                        break;
                    case IMU::Gesture::TiltRight:
                        SharedInstances::serial.serial_debug(My::Config::Debug::UART_ANGLE_SENSOR_TILT_RIGHT, My::Infos::sensor_tilt_right);
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
