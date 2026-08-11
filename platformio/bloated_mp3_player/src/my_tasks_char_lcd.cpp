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
* FILE: my_tasks_char_lcd.cpp
* CREATION DATE: 08-08-2026
* DESCRIPTION:
* Owns the 2004A info panel (char_lcd, 0x27). Runs entirely on its own
* so the slow HD44780 writes never stall the 33ms UI task that drives
* the SPI screen (which used to stay blank while the panel "worked").
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Character LCD info panel task.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <audio.hpp>
#include <environmental.hpp>
#include <profiling.hpp>
#include <sdcard.hpp>
#include "my/tasks.hpp"
#include "my/config.hpp"
#include "my/infos.hpp"
#include "shared_instances.hpp"


namespace My
{
    namespace Tasks
    {
        // ─── Character LCD Task (2004A info panel) ───────────────────────────
        void char_lcd(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(My::Config::CHAR_LCD_REFRESH_MS);
            const char *track = nullptr;
            const char *folder = nullptr;
            uint32_t last_env_poll = 0;
            uint32_t now = 0;
            uint32_t last_i2c_error = 0;
            uint32_t cur_i2c_error = 0;
            bool read = false;
            Environmental::Reading env;

            SharedInstances::serial.serial_print(My::Infos::UART::char_lcd_task_start);

            while (true) {
                PROFILE_BLOCK("char_lcd_tick");

                // Refresh environmental values on the same slow cadence as
                // before; the I2C bus is shared, so take the lock.
                now = millis();
                if (now - last_env_poll >= My::Config::Delays::ENVIRONMENTAL_POLL_INTERVAL_MS) {
                    last_env_poll = now;
                    if (SharedInstances::i2c_bus_lock != nullptr) {
                        xSemaphoreTake(SharedInstances::i2c_bus_lock, portMAX_DELAY);
                    }
                    read = SharedInstances::environmental.read(env);
                    if (SharedInstances::i2c_bus_lock != nullptr) {
                        xSemaphoreGive(SharedInstances::i2c_bus_lock);
                    }
                }

                track = SharedInstances::player.track_name();
                folder = SharedInstances::player.track_folder();

                if (SharedInstances::track_browser.is_browsing()) {
                    // Two-stage browser: album list first, then the tracks
                    // inside the highlighted album.
                    uint32_t total = SharedInstances::track_browser.folder_count();
                    SharedInstances::char_lcd.print_at(0, 0, "%-20.20s", SharedInstances::track_browser.folder_name());
                    if (SharedInstances::track_browser.in_track_stage()) {
                        SharedInstances::char_lcd.print_at(0, 1, "> %-18.18s", SharedInstances::track_browser.track_name());
                        SharedInstances::char_lcd.print_at(0, 2, My::Infos::LCD::track_press_play,
                            (unsigned long)SharedInstances::track_browser.folder_track_count() == 0 ? 0 : (unsigned long)(SharedInstances::track_browser.track_index() - SDCard::get_folder(SharedInstances::track_browser.folder_index())->first_track) + 1,
                            (unsigned long)SharedInstances::track_browser.folder_track_count());
                    } else {
                        SharedInstances::char_lcd.print_at(0, 1, "%-20.20s", SharedInstances::track_browser.track_name());
                        SharedInstances::char_lcd.print_at(0, 2, My::Infos::LCD::sel_press_enter,
                            (unsigned long)SharedInstances::track_browser.folder_index() + 1,
                            (unsigned long)total);
                    }
                    SharedInstances::char_lcd.print_at(0, 3, "Vol:%3lu Up:%02lu:%02lu:%02lu",
                        (unsigned long)(SharedInstances::audio.getVolume() * 100 / My::Config::AUDIO_VOLUME_MAX),
                        (unsigned long)(millis() / 1000 / 3600),
                        (unsigned long)((millis() / 1000 / 60) % 60),
                        (unsigned long)(millis() / 1000 % 60));
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_CHAR_LCD_REFRESH,
                        My::Infos::UART::char_lcd_refresh,
                        SharedInstances::track_browser.folder_name()
                    );
                } else if (track && track[0]) {
                    SharedInstances::char_lcd.print_at(0, 0, "%-20.20s", track);
                    SharedInstances::char_lcd.print_at(0, 1, "%-20.20s", folder && folder[0] ? folder : "");
                } else {
                    SharedInstances::char_lcd.print_at(0, 0, "%-20s", My::Infos::LCD::no_track);
                    SharedInstances::char_lcd.print_at(0, 1, "%-20s", "");
                }
                SharedInstances::serial.serial_debug(
                    My::Config::Debug::UART_CHAR_LCD_REFRESH,
                    My::Infos::UART::char_lcd_refresh,
                    track && track[0] ? track : "No track"
                );

                if (read) {
                    SharedInstances::char_lcd.print_at(0, 2, "T:%.1f H:%.0f%% P:%.0fh", env.temperature, env.humidity, env.pressure);
                } else {
                    SharedInstances::char_lcd.print_at(0, 2, "%-20s", My::Infos::LCD::no_sensor);
                }

                SharedInstances::char_lcd.print_at(0, 3, "Vol:%3lu Up:%02lu:%02lu:%02lu",
                    (unsigned long)(SharedInstances::audio.getVolume() * 100 / My::Config::AUDIO_VOLUME_MAX),
                    (unsigned long)(millis() / 1000 / 3600),
                    (unsigned long)((millis() / 1000 / 60) % 60),
                    (unsigned long)(millis() / 1000 % 60));

                // Report I2C trouble with the info panel only when it appears
                // or changes, so a dead bus yields one line, not spam.
                cur_i2c_error = SharedInstances::char_lcd.last_i2c_error();
                if (cur_i2c_error != last_i2c_error) {
                    last_i2c_error = cur_i2c_error;
                    if (cur_i2c_error != 0) {
                        SharedInstances::serial.serial_debug(
                            My::Config::Debug::UART_CHAR_LCD_I2C_ERROR,
                            My::Infos::UART::char_lcd_i2c_error,
                            cur_i2c_error,
                            (unsigned int)My::Config::CHAR_LCD_I2C_ADDR
                        );
                    }
                }

                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
