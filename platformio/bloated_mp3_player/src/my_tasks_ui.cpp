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
#include "my/tasks.hpp"
#include "my/config.hpp"
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
                    SharedInstances::serial.serial_print("[UI] Failed to read environemental values.");
                }
            }
            if (*read) {
                SharedInstances::lcd.setFont(My::Config::FONT_INFO);
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::TEMPERATURE_X, My::Config::DisplayLayout::HUMIDITY_Y, "T: %.1f°C", env->temperature);
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::HUMIDITY_X, My::Config::DisplayLayout::HUMIDITY_Y, "H: %.0f%%", env->humidity);
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::PRESSURE_X, My::Config::DisplayLayout::PRESSURE_Y, "P: %.0fhPa", env->pressure);
                SharedInstances::serial.serial_debug(My::Config::Debug::UART_BMP280_COMPUTED_MODULE_DATA, "[UI] Temp: %.1f C, Hum:  %.0f %%, Pres: %.0f hPa", env->temperature, env->humidity, env->pressure);
                if (SharedInstances::environmental.has_bmp280()) {
                    SharedInstances::serial.serial_debug(
                        My::Config::Debug::UART_BMP280_RAW_MODULE_DATA,
                        "[UI] BMP280 adc_P=%u adc_T=%u p=%lld press=%.1f hPa",
                        SharedInstances::environmental.get_last_adc_p(),
                        SharedInstances::environmental.get_last_adc_t(),
                        (long long)SharedInstances::environmental.get_last_intermediate_p(),
                        env->pressure
                    );
                }
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);
            }
        }

        // ─── UI Task ──────────────────────────────────────────────────────────
        void ui(void *pvParameters)
        {
            (void)pvParameters;
            TickType_t xLastWake = xTaskGetTickCount();
            const TickType_t freq = pdMS_TO_TICKS(33);
            SharedInstances::serial.serial_print("[UI] Mostly harmless.");
            Environmental::Reading env;
            bool read = false;
            uint32_t last_poll = 0;

            while (true) {
                SharedInstances::lcd.clear();
                SharedInstances::lcd.setFont(My::Config::FONT_TITLE);
                SharedInstances::lcd.printAt("Bloated MP3 v1\u25e60", My::Config::DisplayLayout::TITLE_X, My::Config::DisplayLayout::TITLE_Y);
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);

                refresh_environemental_values(&env, &read, &last_poll);

                SharedInstances::lcd.printAt(My::Config::DisplayLayout::UPTIME_X, My::Config::DisplayLayout::UPTIME_Y, "Uptime: %lus", millis() / 1000);

                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::lcd.printAt(My::Config::DisplayLayout::AUDIO_STATUS_X, My::Config::DisplayLayout::AUDIO_STATUS_Y, "|| PAUSE");
                } else {
                    SharedInstances::lcd.printAt(My::Config::DisplayLayout::AUDIO_STATUS_X, My::Config::DisplayLayout::AUDIO_STATUS_Y, ">> PLAY");
                }

                SharedInstances::lcd.drawRect(My::Config::DisplayLayout::RECTANGLE_X, My::Config::DisplayLayout::RECTANGLE_Y, My::Config::DisplayLayout::RECTANGLE_W, My::Config::DisplayLayout::RECTANGLE_H);
                SharedInstances::lcd.fillRect(My::Config::DisplayLayout::RECTANGLE_FILL_X, My::Config::DisplayLayout::RECTANGLE_FILL_Y, ((millis() / 100) % 100) * 124 / 100, My::Config::DisplayLayout::RECTANGLE_FILL_H);

                SharedInstances::lcd.display();
                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
