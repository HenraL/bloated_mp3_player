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
* FILE: my_tasks_ui.cpp
* CREATION DATE: 17-07-2026
* LAST Modified: 11:58:30 23-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file that will define the tasks referenced in the headerfile for the ui section.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include <audio.hpp>
#include <environmental.hpp>
#include <profiling.hpp>
#include "my/tasks.hpp"
#include "my/config.hpp"
#include "my/infos.hpp"
#include "shared_instances.hpp"


namespace My
{
    namespace Tasks
    {
        static void refresh_environemental_values(Environmental::Reading *env, bool *read, uint32_t *last_poll)
        {
            uint32_t now = millis();
            if (now - *last_poll >= My::Config::Delays::ENVIRONMENTAL_POLL_INTERVAL_MS) {
                *last_poll = now;
                *read = SharedInstances::environmental.read(*env);
                if (!*read) {
                    SharedInstances::serial.serial_print(My::Infos::ui_failed_environmental);
                }
            }
            if (*read) {
                SharedInstances::lcd.setFont(My::Config::FONT_INFO);
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::TEMPERATURE_X, My::Config::DisplayLayout::HUMIDITY_Y, "T: %.1f°C", env->temperature);
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::HUMIDITY_X, My::Config::DisplayLayout::HUMIDITY_Y, "H: %.0f%%", env->humidity);
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::PRESSURE_X, My::Config::DisplayLayout::PRESSURE_Y, "P: %.0fhPa", env->pressure);
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_BMP280_COMPUTED_MODULE_DATA, My::Infos::ui_environmental, env->temperature, env->humidity, env->pressure);
                if (SharedInstances::environmental.has_bmp280()) {
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_BMP280_RAW_MODULE_DATA,
                        My::Infos::ui_bmp280_raw,
                        SharedInstances::environmental.get_last_adc_p(),
                        SharedInstances::environmental.get_last_adc_t(),
                        (long long)SharedInstances::environmental.get_last_intermediate_p(),
                        env->pressure
                    );
                }
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);
            }
        }

        // ─── Character LCD (cash-register display) ─────────────────────────
        static void refresh_char_lcd(const Environmental::Reading *env, bool read, uint32_t *last_refresh, uint8_t *last_i2c_error)
        {
            uint32_t now = millis();
            const char *track = SharedInstances::player.track_name();
            uint8_t cur_i2c_error = 0;
            if (now - *last_refresh < My::Config::CHAR_LCD_REFRESH_MS) {
                return;
            }
            *last_refresh = now;

            if (track && track[0]) {
                SharedInstances::char_lcd.print_at(0, 0, "%-20.20s", track);
            } else {
                SharedInstances::char_lcd.print_at(0, 0, "%-20s", "No track");
            }
            SharedInstances::serial.serial_debug(
                My::Config::Debug::UART_CHAR_LCD_REFRESH,
                My::Infos::char_lcd_refresh,
                track && track[0] ? track : "No track"
            );

            if (read) {
                SharedInstances::char_lcd.print_at(0, 1, "T:%.1f H:%.0f%%", env->temperature, env->humidity);
                SharedInstances::char_lcd.print_at(0, 2, "P:%.0fhPa", env->pressure);
            } else {
                SharedInstances::char_lcd.print_at(0, 1, "No sensor data");
            }

            SharedInstances::char_lcd.print_at(0, 3, "Vol:%3lu Up:%lus",
                (unsigned long)(SharedInstances::audio.getVolume() * 100 / My::Config::AUDIO_VOLUME_MAX),
                (unsigned long)(millis() / 1000));

            // Report I2C trouble with the info panel only when it appears
            // or changes, so a dead bus yields one line, not spam.
            cur_i2c_error = SharedInstances::char_lcd.last_i2c_error();
            if (cur_i2c_error != *last_i2c_error) {
                *last_i2c_error = cur_i2c_error;
                if (cur_i2c_error != 0) {
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_CHAR_LCD_I2C_ERROR,
                        My::Infos::char_lcd_i2c_error,
                        cur_i2c_error,
                        (unsigned int)My::Config::CHAR_LCD_I2C_ADDR
                    );
                }
            }
        }

        // ─── UI Task ──────────────────────────────────────────────────────────
        void ui(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(33);
            SharedInstances::serial.serial_print(My::Infos::ui_mostly_harmless);
            Environmental::Reading env;
            bool read = false;
            uint32_t last_poll = 0;
            uint32_t last_char_lcd = 0;
            uint8_t last_char_lcd_i2c_error = 0;

while (true) {
                PROFILE_BLOCK("ui_tick");
                SharedInstances::lcd.clear();
                SharedInstances::lcd.setFont(My::Config::FONT_TITLE);
                SharedInstances::lcd.printAt("Bloated MP3 v1\u25e60", My::Config::DisplayLayout::TITLE_X, My::Config::DisplayLayout::TITLE_Y);
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);

                refresh_environemental_values(&env, &read, &last_poll);
                refresh_char_lcd(&env, read, &last_char_lcd, &last_char_lcd_i2c_error);

                SharedInstances::lcd.printAt(My::Config::DisplayLayout::UPTIME_X, My::Config::DisplayLayout::UPTIME_Y, "Uptime: %lus", millis() / 1000);

                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::lcd.printAt(My::Config::DisplayLayout::AUDIO_STATUS_X, My::Config::DisplayLayout::AUDIO_STATUS_Y, "|| PAUSE");
                } else {
                    SharedInstances::lcd.printAt(My::Config::DisplayLayout::AUDIO_STATUS_X, My::Config::DisplayLayout::AUDIO_STATUS_Y, ">> PLAY");
                }

                SharedInstances::lcd.drawRect(My::Config::DisplayLayout::RECTANGLE_X, My::Config::DisplayLayout::RECTANGLE_Y, My::Config::DisplayLayout::RECTANGLE_W, My::Config::DisplayLayout::RECTANGLE_H);
                SharedInstances::lcd.fillRect(My::Config::DisplayLayout::RECTANGLE_FILL_X, My::Config::DisplayLayout::RECTANGLE_FILL_Y, ((millis() / 100) % 100) * 124 / 100, My::Config::DisplayLayout::RECTANGLE_FILL_H);

                SharedInstances::lcd.display();
                if (My::Config::Debug::LCD_ONESHOT_TEST_ENABLED) {
                    // Static one-shot screen: draw once, then idle forever so
                    // any flicker observed is from wiring, not software.
                    while (true) {
                        vTaskDelay(pdMS_TO_TICKS(1000));
                    }
                }
                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
