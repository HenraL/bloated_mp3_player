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
* LAST Modified: 19:35:9 22-07-2026
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
                SharedInstances::lcd.printAt(2, 40, "T: %.1f°C", env->temperature);
                SharedInstances::lcd.printAt(52, 40, "Hum:  %.0f %%", env->humidity);
                SharedInstances::lcd.printAt(72, 40, "Pres: %.0f hPa", env->pressure);
                SharedInstances::serial.serial_print("[UI] Temp: %.1f C, Hum:  %.0f %%, Pres: %.0f hPa", env->temperature, env->humidity, env->pressure);
                if (SharedInstances::environmental.has_bmp280()) {
                    SharedInstances::serial.serial_print(
                        "[UI] BMP280 adc_P=%u adc_T=%u p=%lld press=%.1f hPa",
                        SharedInstances::environmental.get_last_adc_p(),
                        SharedInstances::environmental.get_last_adc_t(),
                        (long long)SharedInstances::environmental.get_last_intermediate_p(),
                        env->pressure);
                }
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
                SharedInstances::lcd.printAt("Bloated MP3 v1.0", 0, 12);
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);

                refresh_environemental_values(&env, &read, &last_poll);

                SharedInstances::lcd.printAt(0, 50, "Uptime: %lus", millis() / 1000);

                if (SharedInstances::audio.getStatus() == Audio::Playing) {
                    SharedInstances::lcd.printAt(70, 24, "|| PAUSE");
                } else {
                    SharedInstances::lcd.printAt(70, 24, ">> PLAY");
                }

                SharedInstances::lcd.drawRect(0, 54, 128, 8);
                SharedInstances::lcd.fillRect(2, 56, ((millis() / 100) % 100) * 124 / 100, 4);

                SharedInstances::lcd.display();
                vTaskDelayUntil(&xLastWake, freq);
            }
        }
    } // namespace Tasks
} // namespace My
