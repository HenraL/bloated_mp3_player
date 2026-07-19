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
* FILE: my_threads.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 21:15:38 17-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that contains the implementation of the thread logics while keeping them as a shared instance.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "my/config.hpp"
#include "my/serial.hpp"
#include "my/tasks.hpp"
#include "my/threads.hpp"

namespace My
{
    Threads::Threads() {};
    Threads::~Threads() {};
    void Threads::initialise_ui()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::ui,
            My::Config::Priorities::UI::PROCESS_NAME,
            My::Config::Priorities::UI::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::UI::TASK_PRIORITY,
            &_handle_ui,
            My::Config::Priorities::UI::X_CORE_ID
        );
    }
    void Threads::initialise_audio()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::audio,
            My::Config::Priorities::Audio::PROCESS_NAME,
            My::Config::Priorities::Audio::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::Audio::TASK_PRIORITY,
            &_handle_audio,
            My::Config::Priorities::Audio::X_CORE_ID
        );
    }
    void Threads::initialise_sensors()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::sensor,
            My::Config::Priorities::Sensor::PROCESS_NAME,
            My::Config::Priorities::Sensor::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::Sensor::TASK_PRIORITY,
            &_handle_sensor,
            My::Config::Priorities::Sensor::X_CORE_ID
        );
    }
    void Threads::initialise_led()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::led,
            My::Config::Priorities::LED::PROCESS_NAME,
            My::Config::Priorities::LED::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::LED::TASK_PRIORITY,
            &_handle_led,
            My::Config::Priorities::LED::X_CORE_ID
        );
    }
    void Threads::initialise_matrix()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::matrix,
            My::Config::Priorities::Matrix::PROCESS_NAME,
            My::Config::Priorities::Matrix::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::Matrix::TASK_PRIORITY,
            &_handle_matrix,
            My::Config::Priorities::Matrix::X_CORE_ID
        );
    }
    void Threads::initialise_input()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::input,
            My::Config::Priorities::Input::PROCESS_NAME,
            My::Config::Priorities::Input::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::Input::TASK_PRIORITY,
            &_handle_input,
            My::Config::Priorities::Input::X_CORE_ID
        );
    }
    void Threads::initialise_serial()
    {
        xTaskCreatePinnedToCore(
            My::Tasks::serial,
            My::Config::Priorities::Serial::PROCESS_NAME,
            My::Config::Priorities::Serial::US_STACK_DEPTH,
            NULL,
            My::Config::Priorities::Serial::TASK_PRIORITY,
            &_handle_serial,
            My::Config::Priorities::Serial::X_CORE_ID
        );
    }

}
