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
* FILE: english.hpp
* CREATION DATE: 07-08-2026
* DESCRIPTION:
* English message pack. Included by my/infos.hpp when no language macro
* is defined (the default tongue of the galaxy... en is).
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: English messages for the whole program.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once

namespace My
{
    namespace Infos
    {
        // ─── Boot ────────────────────────────────────────────────────────────────
        static const char boot_title[] = "Bloated MP3 Player -- DON'T PANIC";
        static const char quote_ships_bricks[] = "The ships hung in the sky in much the same way that bricks don't.";
        static const char all_tasks_spawned[] = "All tasks spawned. Entering the infinite improbability loop.";

        // ─── Warnings (hardware divorces & misc) ──────────────────────────────────
        static const char warn_sd_not_mounted[] = "WARN: SD card not mounted, not scanning for audio tracks.";
        static const char warn_environmental[] = "WARN: AHT20+BMP280 -- the answer is 42, but the sensor is 0. Gone where the Vogons would send a badly-written poem.";
        static const char warn_imu[] = "WARN: MPU6050 -- we apologize for the inconvenience.";
        static const char warn_sd_card[] = "WARN: SD card -- a common mistake that people make when trying to design something completely foolproof is to underestimate the ingenuity of complete fools.";
        static const char warn_no_music[] = "WARN: No music found -- ";
        static const char warn_i2s[] = "WARN: I2S -- in the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move. (err 0x%lx)";

        // ─── SD discover ───────────────────────────────────────────────────────────
        static const char sd_scanning[] = "[SD] Scanning for audio tracks...";
        static const char sd_found[] = "[SD] Found %lu tracks.";

        // ─── UI task ───────────────────────────────────────────────────────────────
        static const char ui_mostly_harmless[] = "[UI] Mostly harmless.";
        static const char ui_failed_environmental[] = "[UI] Failed to read environemental values.";
        static const char ui_environmental[] = "[UI] Temp: %.1f C, Hum: %.0f %%, Pres: %.0f hPa";
        static const char ui_bmp280_raw[] = "[UI] BMP280 adc_P=%u adc_T=%u p=%lld press=%.1f hPa";

        // ─── Sensor task ───────────────────────────────────────────────────────────
        static const char sensor_time_illusion[] = "[Sensor] Time is an illusion. Lunchtime doubly so.";
        static const char sensor_shake[] = "[Sensor] Shake";
        static const char sensor_tilt_left[] = "[Sensor] TiltLeft";
        static const char sensor_tilt_right[] = "[Sensor] TiltRight";

        // ─── LED task ──────────────────────────────────────────────────────────────
        static const char led_light_twice_bright[] = "[LED] The light that burns twice as bright...";
        static const char led_stack_hwm[] = "[LED] Stack high-water mark: %u bytes free";

        // ─── Matrix task ───────────────────────────────────────────────────────────
        static const char matrix_light_twice_bright[] = "[MATRIX] The array of lights that burn twice as bright...";

        // ─── Audio task ───────────────────────────────────────────────────────────
        static const char audio_so_long_fish[] = "[Audio] So long, and thanks for all the fish.";
        static const char audio_status_change[] = "[Audio] status -> %s";
        static const char audio_i2s_stalled[] = "[Audio] I2S output stalled (err 0x%lx) -- DMA not draining, no sound";
        static const char audio_stack_hwm[] = "[Audio] Stack high-water mark: %u bytes free";
        static const char audio_tick_failed[] = "[Audio] tick=%d [%s]";
        static const char audio_no_player[] = "[Audio] No players are loaded.";
        static const char audio_not_playing[] = "[Audio] Audio is not playing.";

        // ─── Input task ───────────────────────────────────────────────────────────
        static const char input_dont_panic[] = "[INPUT] Don't Panic.";
        static const char input_sd_not_mounted[] = "[INPUT] sd card is not mounted.";
        static const char input_no_tracks[] = "[INPUT] There are no tracks to play.";
        static const char input_all_tracks_played[] = "[INPUT] we have played all the tracks and will reset the current track index.";
        static const char input_playing_track[] = "[INPUT] Playing track %lu";
        static const char input_filepath[] = "[INPUT] filepath: %s";
        static const char input_raw_a_b[] = "[INPUT] RAW A=%u B=%u";
        static const char input_raw_sw[] = "[INPUT] RAW SW=%u";
        static const char input_clicky_value[] = "[INPUT] Clicky potentiometer value: %d";
        static const char input_volume[] = "[INPUT] Volume: %u";
        static const char input_clicky_zero[] = "[INPUT] The clicky pot has not been turned.";
        static const char input_switch_pressed[] = "[INPUT] The rotary switch was pressed.";
        static const char input_double_pressed[] = "[INPUT] The rotary switch was double pressed, skipping track.";
        static const char input_triple_pressed[] = "[INPUT] Rotated switch was triple pressed, going back a track.";
        static const char input_ultrasonic_pressed[] = "[INPUT] ultrasonic is pressed.";
        static const char input_swipe_value[] = "[INPUT] Ultrasonic swipe value: %d";
        static const char input_swiped[] = "[INPUT] Ultrasonic was swiped.";
        static const char input_track_finished[] = "[INPUT] Track finished, advancing to the next track.";

        // ─── Vogon panel task ─────────────────────────────────────────────────────
        static const char vogon_quote[] = "[VOGON] %s";
        static const char vogon_eom_enter[] = "[VOGON] End-of-poem blip";
        static const char vogon_stanza_switch[] = "[VOGON] Stanza switch, next stanza %u";
        static const char vogon_i2c_error[] = "[VOGON] I2C write error %u (addr 0x%02X)";

        // ─── Character LCD (info panel) ────────────────────────────────────────────
        static const char char_lcd_refresh[] = "[CHAR] Refresh: '%s'";
        static const char char_lcd_i2c_error[] = "[CHAR] I2C write error %u (addr 0x%02X)";
    } // namespace Infos
} // namespace My