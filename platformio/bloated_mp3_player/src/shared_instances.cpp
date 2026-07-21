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
* FILE: shared_instances.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 23:1:18 20-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file in charge of storing the pointers that will be used in the program.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "shared_instances.hpp"
#include "my/config/pins.hpp"

namespace SharedInstances
{
    U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI u8g2_lcd(U8G2_R0, My::Config::Pins::LCD_CS, My::Config::Pins::LCD_DC, My::Config::Pins::LCD_RST);
    Screen display(u8g2_lcd);
    LcdCanvas lcd(display);
    MatrixCanvas matrix_cvs;
    My::LED::LED onboard(1, My::Config::Pins::ONBOARD_LED_PIN, NEO_GRB + NEO_KHZ800);
    My::Serial serial(SERIAL_MSG_LEN, SERIAL_QUEUE_LEN, My::Config::UART_BAUD);
    My::Threads threads;
}
