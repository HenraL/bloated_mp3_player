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
* FILE: imu.hpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Tilt / gyroscope / accelerometer abstraction. If the device feels
* tilted, it may decide to skip to the next track, activate party mode,
* or simply tell you to hold it level, you absolute muffin.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Orientation and shake detection.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <Arduino.h>

namespace IMU
{
    struct Vec3
    {
        float x, y, z;
    };

    struct Orientation
    {
        float roll, pitch, yaw;
    };

    enum class Gesture : uint8_t
    {
        None,
        Shake,
        TiltLeft,
        TiltRight,
        TiltForward,
        TiltBackward,
        FaceUp,
        FaceDown
    };

    bool begin(uint8_t sda_pin, uint8_t scl_pin);
    bool read_accel(Vec3 &out);
    bool read_gyro(Vec3 &out);
    Orientation compute_orientation(const Vec3 &accel);

    Gesture gesture_tick();
    void set_shake_threshold(float threshold);
    void set_tilt_threshold(float degrees);

    Vec3 get_gravity();
}
