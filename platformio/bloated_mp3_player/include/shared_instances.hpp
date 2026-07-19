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
* FILE: shared_instances.hpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:19:49 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file in charge of storing the pointers that will be used in the program.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#pragma once
#include <U8g2lib.h>
#include <screen.hpp>
#include <uilcd.hpp>
#include <matrix.hpp>
#include <uimatrix.hpp>
#include <Adafruit_NeoPixel.h>
#include "my.hpp"

namespace SharedInstances
{
    extern U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2_lcd;
    extern Screen display;
    extern LcdCanvas lcd;
    extern MatrixCanvas matrix_cvs;
    extern Adafruit_NeoPixel onboard;
    extern My::Serial serial;
}
