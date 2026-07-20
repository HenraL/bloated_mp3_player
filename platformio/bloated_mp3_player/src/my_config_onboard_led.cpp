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
* FILE: my_config_onboard_led.cpp
* CREATION DATE: 21-07-2026
* LAST Modified: 1:43:24 21-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the cpp file for the onboard led configuration.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#include "my/config/onboard_led.hpp"

// Messages for the onboard depression

// "DON'T PANIC"
// A comforting boot message
static const My::Config::Structures::LEDStep message_dont_panic[] PROGMEM =
{
    // D (-..)
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,       My::Config::onboard_duration_off},
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,       My::Config::onboard_duration_off},
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,       My::Config::onboard_duration_letter_gap},

    // O (---)
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,       My::Config::onboard_duration_off},
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,       My::Config::onboard_duration_off},
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,       My::Config::onboard_duration_letter_gap},

    // N (-.)
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,       My::Config::onboard_duration_off},
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,       My::Config::onboard_duration_letter_gap},

    // T (-)
    {My::LED::Colours::DeepSkyBlue, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,       My::Config::onboard_duration_word_gap},


    // P (.--.)
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_letter_gap},

    // A (.-)
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,     My::Config::onboard_duration_letter_gap},

    // N (-.)
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_letter_gap},

    // I (..)
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_letter_gap},

    // C (-.-.)
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,     My::Config::onboard_duration_off},
    {My::LED::Colours::LimeGreen, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,     My::Config::onboard_duration_eom}
};


// "READY"
static const My::Config::Structures::LEDStep message_ready[] PROGMEM =
{
    {My::LED::Colours::Green, My::Config::onboard_duration_short},
    {My::LED::Colours::Black, My::Config::onboard_duration_off / 2},
    {My::LED::Colours::Green, My::Config::onboard_duration_short},
    {My::LED::Colours::Black, My::Config::onboard_duration_off / 2},
    {My::LED::Colours::Green, My::Config::onboard_duration_medium},

    {My::LED::Colours::Black, My::Config::onboard_duration_long}
};


// "THINKING"
static const My::Config::Structures::LEDStep message_thinking[] PROGMEM =
{
    {My::LED::Colours::Gold,  My::Config::onboard_duration_short},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Gold,  My::Config::onboard_duration_short},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Gold,  My::Config::onboard_duration_short},
    {My::LED::Colours::Black, My::Config::onboard_duration_long}
};


// "ERROR"
static const My::Config::Structures::LEDStep message_error[] PROGMEM =
{
    {My::LED::Colours::Red,   My::Config::onboard_duration_long},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Red,   My::Config::onboard_duration_long},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Red,   My::Config::onboard_duration_long},
    {My::LED::Colours::Black, My::Config::onboard_duration_eom}
};


// "SLEEP"
static const My::Config::Structures::LEDStep message_sleep[] PROGMEM =
{
    {My::LED::Colours::MidnightBlue, My::Config::onboard_duration_long},
    {My::LED::Colours::Black,        My::Config::onboard_duration_idle}
};


// "42"
// (..--- ..---)
static const My::Config::Structures::LEDStep message_42[] PROGMEM =
{
    {My::LED::Colours::RebeccaPurple, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,         My::Config::onboard_duration_off},
    {My::LED::Colours::RebeccaPurple, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,         My::Config::onboard_duration_off},
    {My::LED::Colours::RebeccaPurple, My::Config::onboard_duration_da},
    {My::LED::Colours::Black,         My::Config::onboard_duration_word_gap},

    {My::LED::Colours::RebeccaPurple, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,         My::Config::onboard_duration_off},
    {My::LED::Colours::RebeccaPurple, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black,         My::Config::onboard_duration_off},
    {My::LED::Colours::RebeccaPurple, My::Config::onboard_duration_da},

    {My::LED::Colours::Black,         My::Config::onboard_duration_eom}
};


// This is the onboard_message array through which the esp will run because why not.
const My::Config::Structures::LEDMessage My::Config::onboard_messages[] PROGMEM = {
    {message_dont_panic, sizeof(message_dont_panic) / sizeof(message_dont_panic[0])},
    {message_ready,      sizeof(message_ready) / sizeof(message_ready[0])},
    {message_thinking,   sizeof(message_thinking) / sizeof(message_thinking[0])},
    {message_error,      sizeof(message_error) / sizeof(message_error[0])},
    {message_sleep,      sizeof(message_sleep) / sizeof(message_sleep[0])},
    {message_42,         sizeof(message_42) / sizeof(message_42[0])},

    {nullptr, 0}
};

// End of message animation
static const My::Config::Structures::LEDStep message_eom[] PROGMEM = {
    {My::LED::Colours::Gold, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Gold, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Gold, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black, My::Config::onboard_duration_off},

    {My::LED::Colours::Gold, My::Config::onboard_duration_dit},
    {My::LED::Colours::Black, My::Config::onboard_duration_eom}
};

const My::Config::Structures::LEDMessage My::Config::onboard_eom[] PROGMEM = {
    {message_eom, sizeof(message_eom) / sizeof(message_eom[0])},

    {nullptr,0}
};
