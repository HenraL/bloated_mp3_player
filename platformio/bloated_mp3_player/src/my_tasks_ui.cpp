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
#include <sdcard.hpp>
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
                if (SharedInstances::i2c_bus_lock != nullptr) {
                    xSemaphoreTake(SharedInstances::i2c_bus_lock, portMAX_DELAY);
                }
                *read = SharedInstances::environmental.read(*env);
                if (SharedInstances::i2c_bus_lock != nullptr) {
                    xSemaphoreGive(SharedInstances::i2c_bus_lock);
                }
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

        // ─── Track browser (shown while browsing) ───────────────────────────────
        static void draw_track_browser(void)
        {
            uint32_t total = SharedInstances::track_browser.folder_count();
            uint32_t sel = SharedInstances::track_browser.folder_index();
            uint32_t first = 0;
            uint32_t i = 0;
            uint32_t idx = 0;
            uint32_t base = 0;
            const SDCard::FolderInfo *fi = nullptr;
            const SDCard::TrackInfo *ti = nullptr;
            const char *name = nullptr;

            if (total == 0) {
                SharedInstances::lcd.printAt(0, My::Config::DisplayLayout::ENVIRONEMENT_POSITION_Y, "No folders");
                return;
            }
            if (SharedInstances::track_browser.in_track_stage()) {
                // Second stage: the tracks of the highlighted album.
                fi = SDCard::get_folder(sel);
                total = fi ? fi->track_count : 0;
                base = fi ? fi->first_track : 0;
                SharedInstances::lcd.printAt(My::Config::DisplayLayout::TITLE_X, My::Config::DisplayLayout::TITLE_Y, "%-16.16s", SharedInstances::track_browser.folder_name());
                if (SharedInstances::track_browser.folder_track_count() > 2) {
                    first = SharedInstances::track_browser.track_index() - base > 2
                            ? SharedInstances::track_browser.track_index() - base - 2 : 0;
                }
                for (i = 0; i < 5 && (first + i) < total; i++) {
                    idx = base + first + i;
                    ti = SDCard::get_track(idx);
                    name = ti ? ti->filename : "?";
                    SharedInstances::lcd.printAt(
                        My::Config::DisplayLayout::TEMPERATURE_X,
                        My::Config::DisplayLayout::ENVIRONEMENT_POSITION_Y + (int16_t)(i * 9),
                        "%s %-15.15s", idx == SharedInstances::track_browser.track_index() ? ">" : " ", name
                    );
                }
                SharedInstances::lcd.printAt(
                    My::Config::DisplayLayout::UPTIME_X,
                    My::Config::DisplayLayout::UPTIME_Y,
                    "Rotate:tr Press:pl"
                );
                return;
            }
            if (sel > 2) {
                first = sel - 2;
            }
            SharedInstances::lcd.printAt(My::Config::DisplayLayout::TITLE_X, My::Config::DisplayLayout::TITLE_Y, "Select folder:");
            for (i = 0; i < 5 && (first + i) < total; i++) {
                idx = first + i;
                fi = SDCard::get_folder(idx);
                name = fi ? fi->folder : "?";
                SharedInstances::lcd.printAt(
                    My::Config::DisplayLayout::TEMPERATURE_X,
                    My::Config::DisplayLayout::ENVIRONEMENT_POSITION_Y + (int16_t)(i * 9),
                    "%s %-15.15s", idx == sel ? ">" : " ", name
                );
            }
            SharedInstances::lcd.printAt(
                My::Config::DisplayLayout::UPTIME_X,
                My::Config::DisplayLayout::UPTIME_Y,
                "Rotate:sel  Press:enter"
            );
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

while (true) {
                PROFILE_BLOCK("ui_tick");
                SharedInstances::lcd.clear();
                SharedInstances::lcd.setFont(My::Config::FONT_TITLE);
                SharedInstances::lcd.printAt("Bloated MP3 v1\u25e60", My::Config::DisplayLayout::TITLE_X, My::Config::DisplayLayout::TITLE_Y);
                SharedInstances::lcd.setFont(My::Config::FONT_BODY);

                if (SharedInstances::track_browser.is_browsing()) {
                    draw_track_browser();
                    SharedInstances::lcd.display();
                    if (My::Config::Debug::LCD_ONESHOT_TEST_ENABLED) {
                        while (true) {
                            vTaskDelay(pdMS_TO_TICKS(1000));
                        }
                    }
                    vTaskDelayUntil(&xLastWake, freq);
                    continue;
                }

                refresh_environemental_values(&env, &read, &last_poll);

                SharedInstances::lcd.printAt(
                    My::Config::DisplayLayout::UPTIME_X,
                    My::Config::DisplayLayout::UPTIME_Y,
                    "Up: %02lu:%02lu:%02lu",
                    (unsigned long)(millis() / 1000 / 3600),
                    (unsigned long)((millis() / 1000 / 60) % 60),
                    (unsigned long)(millis() / 1000 % 60)
                );

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
