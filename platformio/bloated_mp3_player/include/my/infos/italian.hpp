/*
* +==== BEGIN Bloated MP3 Player =================+
* LOGO:
* .......................
* ...><>.............<><.
* ..><>.><>.......<><.<><
* .><>.<></><.><>.<><.<><
* ..><>.><>.......<><.<><
* ...><>.............<><.
* .......................
* /STOP
* PROJECT: Bloated MP3 Player
* FILE: italian.hpp
* DESCRIPTION:
* The Italian translation pack. Me gusta la lingua.
* Select it by defining MY_INFOS_LANG_IT before including my/infos.hpp.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Italian messages for the whole program. Same names as english.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once

namespace My
{
    namespace Infos
    {
        namespace UART
        {
            // ─── Boot ────────────────────────────────────────────────────────────────
            static const char boot_title[] = "Bloated MP3 Player -- NON FARSI PRENDERE DAL PANICO";
            static const char quote_ships_bricks[] = "Le navi restavano appese in cielo proprio come i mattoni non ci restano.";
            static const char all_tasks_spawned[] = "Tutte le task create. Si entra nel cappio di improbabilità infinita.";

            // ─── Bluetooth (ponte UART BLE) ───────────────────────────────────────────
            static const char bt_bridge_ready[] = "[BT] Ponte UART BLE online -- feed di debug duplicato";
            static const char bt_bridge_fail[] = "ATTENZIONE: avvio del ponte UART BLE fallito";

            // ─── Warnings (hardware divorces & misc) ──────────────────────────────────
            static const char warn_sd_not_mounted[] = "ATTENZIONE: scheda SD non montata, nessuna scansione dei brani audio.";
            static const char warn_environmental[] = "ATTENZIONE: AHT20+BMP280 -- la risposta è 42, ma il sensore è 0. Spedito dove i Vogoni manderebbero una poesia scritta male.";
            static const char warn_imu[] = "ATTENZIONE: MPU6050 -- ci scusiamo per il disagio.";
            static const char warn_sd_card[] = "ATTENZIONE: scheda SD -- un errore comune che la gente commette quando cerca di progettare qualcosa di completamente a prova di sciocchi è sottovalutare l'inventiva degli sciocchi più completi.";
            static const char warn_no_music[] = "ATTENZIONE: nessuna musica trovata -- ";
            static const char warn_i2s[] = "ATTENZIONE: I2S -- all'inizio fu creato l'Universo. Questo ha fatto arrabbiare un sacco di persone ed è stato considerato una brutta idea. (err 0x%lx)";

            // ─── SD discover ───────────────────────────────────────────────────────────
            static const char sd_scanning[] = "[SD] Scansione tracce audio in corso...";
            static const char sd_found[] = "[SD] Trovate %lu tracce.";

            // ─── UI task ───────────────────────────────────────────────────────────────
            static const char ui_mostly_harmless[] = "[UI] Perlopiù innocuo.";
            static const char ui_failed_environmental[] = "[UI] Lettura dei valori ambientali fallita.";
            static const char ui_environmental[] = "[UI] Temp: %.1f C, Umid: %.0f %%, Press: %.0f hPa";
            static const char ui_bmp280_raw[] = "[UI] BMP280 adc_P=%u adc_T=%u p=%lld press=%.1f hPa";

            // ─── Sensor task ───────────────────────────────────────────────────────────
            static const char sensor_time_illusion[] = "[Sensor] Il tempo è un'illusione. L'ora di pranzo, però, doppiamente.";
            static const char sensor_shake[] = "[Sensor] Scuotito";
            static const char sensor_tilt_left[] = "[Sensor] InclinatoSx";
            static const char sensor_tilt_right[] = "[Sensor] InclinatoDe";
            static const char sensor_orientation[] = "[Sensor] Rollio: %.1f Beccheggio: %.1f Imbardata: %.1f";

            // ─── LED task ──────────────────────────────────────────────────────────────
            static const char led_light_twice_bright[] = "[LED] La luce che brucia due volte più forte...";
            static const char led_stack_hwm[] = "[LED] Mark d'acque alta dello stack: %u byte liberi";

            // ─── Matrix task ───────────────────────────────────────────────────────────
            static const char matrix_light_twice_bright[] = "[MATRIX] L'array di luci che bruciano due volte più forte...";

            // ─── Audio task ────────────────────────────────────────────────────────────
            static const char audio_so_long_fish[] = "[Audio] Ciao, e grazie per tutto il pesce.";
            static const char audio_status_change[] = "[Audio] stato -> %s";
            static const char audio_i2s_stalled[] = "[Audio] Uscita I2S bloccata (err 0x%lx) -- la DMA non scarica, nessun suono";
            static const char audio_stack_hwm[] = "[Audio] Mark dell'acqua alta dello stack: %u byte liberi";
            static const char audio_tick_failed[] = "[Audio] tick=%d [%s]";
            static const char audio_no_player[] = "[Audio] Nessun player caricato.";
            static const char audio_not_playing[] = "[Audio] L'audio non sta suonando.";

            // ─── Input task ───────────────────────────────────────────────────────────
            static const char input_dont_panic[] = "[INPUT] Non farti prendere dal panico.";
            static const char input_sd_not_mounted[] = "[INPUT] la scheda SD non è montata.";
            static const char input_no_tracks[] = "[INPUT] Non ci sono tracce da riprodurre.";
            static const char input_all_tracks_played[] = "[INPUT] abbiamo riprodotto tutte le tracce e riporteremo l'indice alla traccia corrente.";
            static const char input_playing_track[] = "[INPUT] Traccia %lu in esecuzione";
            static const char input_filepath[] = "[INPUT] percorso: %s";
            static const char input_raw_a_b[] = "[INPUT] RAW A=%u B=%u";
            static const char input_raw_sw[] = "[INPUT] RAW SW=%u";
            static const char input_clicky_value[] = "[INPUT] Valore del potenziometro cliccabile: %d";
            static const char input_volume[] = "[INPUT] Volume: %u";
            static const char input_clicky_zero[] = "[INPUT] Il potenziometro cliccabile non è stato toccato.";
            static const char input_switch_pressed[] = "[INPUT] L'interruttore rotativo è stato premuto.";
            static const char input_double_pressed[] = "[INPUT] Doppia pressione, salto di traccia.";
            static const char input_triple_pressed[] = "[INPUT] Tripla pressione, torna alla traccia precedente.";
            static const char input_ultrasonic_pressed[] = "[INPUT] ultrasonico premuto.";
            static const char input_ultrasonic_double_waved[] = "[INPUT] Doppia onda ultrasonica, ritorno al menu.";
            static const char input_going_back_to_menu[] = "[INPUT] Menu (scelta album).";
            static const char input_swipe_value[] = "[INPUT] Valore gesto ultrasonico: %d";
            static const char input_swiped[] = "[INPUT] Gesto ultrasonico rilevato.";
            static const char input_track_finished[] = "[INPUT] Traccia terminata, passo alla successiva.";
            static const char input_select_start[] = "[INPUT] Sfogliando i brani: ruota per selezionare, premi per riprodurre.";
            static const char input_select_moved[] = "[INPUT] Selezione: %lu/%lu";
            static const char input_select_play[] = "[INPUT] Riproduco il brano selezionato %lu.";

            // ─── Vogon panel task ─────────────────────────────────────────────────────
            static const char vogon_quote[] = "[VOGON] %s";
            static const char vogon_eom_enter[] = "[VOGON] Strofa di fine poesia";
            static const char vogon_stanza_switch[] = "[VOGON] Cambio strofa, prossima strofa %u";
            static const char vogon_i2c_error[] = "[VOGON] Errore I2C %u (indirizzo 0x%02X)";

            // ─── Character LCD (info panel) ────────────────────────────────────────────
            static const char char_lcd_task_start[] = "[CHAR] Il pannello info gira sulla propria task.";
            static const char char_lcd_refresh[] = "[CHAR] Aggiornamento: '%s'";
            static const char char_lcd_i2c_error[] = "[CHAR] Errore I2C %u (indirizzo 0x%02X)";

        } // namespace UART

        // ─── LCD display strings (2004A info panel, 1602A Vogon panel, boot) ───────
        namespace LCD
        {
            static const char boot_name[] = "Bloated MP3";
            static const char boot_motto[] = "NON FARSI PRENDERE";
            static const char booting_i2c[] = "Avvio: I2C...";
            static const char i2c_ok[] = "Sensori I2C: OK";
            static const char i2c_fail[] = "Sensori I2C: ERRORE";
            static const char booting_sd[] = "Avvio: SD...";
            static const char sd_missing[] = "SD: ASSENTE";
            static const char sd_no_music[] = "SD: nessuna musica";
            static const char sd_tracks[] = "SD: %lu brani";
            static const char booting_audio[] = "Avvio: audio...";
            static const char audio_ok[] = "Audio I2S: OK";
            static const char audio_fail[] = "Audio I2S: ERRORE";
            static const char track_press_play[] = "Traccia %lu/%lu=play";
            static const char sel_press_enter[] = "Alb %lu/%lu=invio";
            static const char no_track[] = "Nessuna traccia";
            static const char no_sensor[] = "Nessun sensore";
            static const char vogon_title[] = "POESIA VOGON";
            static const char vogon_tilt[] = "Incl R:%5.1f P:%5.1f";
            static const char vogon_yaw[] = "Imbardata:%5.1f";
        } // namespace LCD
    } // namespace Infos
} // namespace My