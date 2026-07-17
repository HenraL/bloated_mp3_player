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
* FILE: imu.cpp
* CREATION DATE: 15-07-2026
* LAST Modified: 20:39:12 15-07-2026
* DESCRIPTION:
* Reads accelerometer and gyroscope data, then decides whether you
* shook it, tilted it, or just sneezed near it. The MP3 player is
* sensitive. Not as sensitive as the Vogons about poetry, but close.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: IMU driver and gesture recognition.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include <Wire.h>
#include "internal/imu.hpp"

static float shake_threshold = 15.0;
static float tilt_threshold_deg = 20.0;
static IMU::Vec3 gravity = {0, 0, 9.81};
static uint32_t last_gesture_ms = 0;
static const uint32_t gesture_cooldown = 300;

bool IMU::begin(uint8_t sda_pin, uint8_t scl_pin)
{
    Wire.begin(sda_pin, scl_pin);
    Wire.beginTransmission(0x68);
    if (Wire.endTransmission() != 0) return false;
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0);
    if (Wire.endTransmission() != 0) return false;
    return true;
}

bool IMU::read_accel(Vec3 &out)
{
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    if (Wire.endTransmission() != 0) return false;
    Wire.requestFrom(0x68, 6);
    if (Wire.available() < 6) return false;
    int16_t ax = (Wire.read() << 8) | Wire.read();
    int16_t ay = (Wire.read() << 8) | Wire.read();
    int16_t az = (Wire.read() << 8) | Wire.read();
    out.x = ax / 16384.0f;
    out.y = ay / 16384.0f;
    out.z = az / 16384.0f;
    gravity = out;
    return true;
}

bool IMU::read_gyro(Vec3 &out)
{
    Wire.beginTransmission(0x68);
    Wire.write(0x43);
    if (Wire.endTransmission() != 0) return false;
    Wire.requestFrom(0x68, 6);
    if (Wire.available() < 6) return false;
    int16_t gx = (Wire.read() << 8) | Wire.read();
    int16_t gy = (Wire.read() << 8) | Wire.read();
    int16_t gz = (Wire.read() << 8) | Wire.read();
    out.x = gx / 131.0f;
    out.y = gy / 131.0f;
    out.z = gz / 131.0f;
    return true;
}

IMU::Orientation IMU::compute_orientation(const Vec3 &accel)
{
    Orientation o;
    o.roll  = atan2(accel.y, accel.z) * 180.0 / M_PI;
    o.pitch = atan2(-accel.x, sqrt(accel.y * accel.y + accel.z * accel.z)) * 180.0 / M_PI;
    o.yaw   = 0;
    return o;
}

void IMU::set_shake_threshold(float t)
{
    shake_threshold = t;
}

void IMU::set_tilt_threshold(float degrees)
{
    tilt_threshold_deg = degrees;
}

IMU::Vec3 IMU::get_gravity()
{
    return gravity;
}

IMU::Gesture IMU::gesture_tick()
{
    uint32_t now = millis();
    if (now - last_gesture_ms < gesture_cooldown)
        return Gesture::None;
    last_gesture_ms = now;

    Vec3 accel;
    if (!read_accel(accel)) return Gesture::None;

    float mag = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
    if (mag > shake_threshold)
        return Gesture::Shake;

    Orientation o = compute_orientation(accel);
    if (o.pitch > tilt_threshold_deg)    return Gesture::TiltBackward;
    if (o.pitch < -tilt_threshold_deg)   return Gesture::TiltForward;
    if (o.roll > tilt_threshold_deg)     return Gesture::TiltRight;
    if (o.roll < -tilt_threshold_deg)    return Gesture::TiltLeft;

    if (accel.z > 8.0) return Gesture::FaceUp;
    if (accel.z < -8.0) return Gesture::FaceDown;

    return Gesture::None;
}
