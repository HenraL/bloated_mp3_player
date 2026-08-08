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
* FILE: french.hpp
* DESCRIPTION:
* The French translation pack. Viva la France.
* Select it by defining MY_INFOS_LANG_FR before including my/infos.hpp.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: French messages for the whole program. Same names as english.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once

namespace My
{
    namespace Infos
    {
        // ─── Boot ────────────────────────────────────────────────────────────────
        static const char boot_title[] = "Bloated MP3 Player -- PAS DE PANIQUE";
        static const char quote_ships_bricks[] = "Les vaisseaux restaient suspendus dans le ciel de la même manière que les briques, c'est-à-dire pas.";
        static const char all_tasks_spawned[] = "Toutes les tâches sont lancées. Entrée dans la boucle d'improbabilité infinie.";

        // ─── Warnings (hardware divorces & misc) ──────────────────────────────────
        static const char warn_sd_not_mounted[] = "WARN: carte SD non montée, pas d'analyse des pistes audio.";
        static const char warn_environmental[] = "WARN: AHT20-BMP280 ont fusionné, mais la réponse est 42 alors que le capteur vaut 0. Il est parti là où les Vogons enverraient un poème mal écrit.";
        static const char warn_imu[] = "WARN: MPU6050 -- toutes nos excuses pour le désagrément.";
        static const char warn_sd_card[] = "WARN: carte SD -- une erreur courante quand on conçoit quelque chose d'absolument infaillible est de sous-estimer l'ingéniosité des imbéciles absolus.";
        static const char warn_no_music[] = "WARN: aucune musique trouvée -- ";
        static const char warn_i2s[] = "WARN: I2S -- au commencement l'Univers fut créé. Cela a rendu beaucoup de gens très en colère et a été généralement considéré comme une très mauvaise idée. (err 0x%lx)";

        // ─── SD discover ───────────────────────────────────────────────────────────
        static const char sd_scanning[] = "[SD] Analyse des pistes audio...";
        static const char sd_found[] = "[SD] %lu pistes trouvées.";

        // ─── UI task ───────────────────────────────────────────────────────────────
        static const char ui_mostly_harmless[] = "[UI] Globalement inoffensif.";
        static const char ui_failed_environmental[] = "[UI] Échec de la lecture des valeurs environnementales.";
        static const char ui_environmental[] = "[UI] Temp: %.1f C, Hum: %.0f %%, Press: %.0f hPa";
        static const char ui_bmp280_raw[] = "[UI] BMP280 adc_P=%u adc_T=%u p=%lld press=%.1f hPa";

        // ─── Sensor task ───────────────────────────────────────────────────────────
        static const char sensor_time_illusion[] = "[Sensor] Le temps est une illusion. Le déjeuner, doublement.";
        static const char sensor_shake[] = "[Sensor] Secousse";
        static const char sensor_tilt_left[] = "[Sensor] InclinaisonG";
        static const char sensor_tilt_right[] = "[Sensor] InclinaisonD";

        // ─── LED task ──────────────────────────────────────────────────────────────
        static const char led_light_twice_bright[] = "[LED] La lumière qui brûle deux fois plus fort...";
        static const char led_stack_hwm[] = "[LED] Marqueur d'eau haute de la pile : %u octets libres";

        // ─── Matrix task ───────────────────────────────────────────────────────────
        static const char matrix_light_twice_bright[] = "[MATRIX] Le réseau de lumières qui brûlent deux fois plus fort...";

        // ─── Audio task ────────────────────────────────────────────────────────────
        static const char audio_so_long_fish[] = "[Audio] Au revoir, et merci pour le poisson.";
        static const char audio_status_change[] = "[Audio] état -> %s";
        static const char audio_i2s_stalled[] = "[Audio] Sortie I2S bloquée (err 0x%lx) -- la DMA ne se vide pas, pas de son";
        static const char audio_stack_hwm[] = "[Audio] Marqueur d'eau haute de la pile : %u octets libres";
        static const char audio_tick_failed[] = "[Audio] beat=%d [%s]";
        static const char audio_no_player[] = "[Audio] Aucun lecteur chargé.";
        static const char audio_not_playing[] = "[Audio] L'audio ne joue pas.";

        // ─── Input task ───────────────────────────────────────────────────────────
        static const char input_dont_panic[] = "[INPUT] Pas de panique.";
        static const char input_sd_not_mounted[] = "[INPUT] la carte SD n'est pas montée.";
        static const char input_no_tracks[] = "[INPUT] Aucune piste à jouer.";
        static const char input_all_tracks_played[] = "[INPUT] toutes les pistes ont été jouées, retour à la piste de départ.";
        static const char input_playing_track[] = "[INPUT] Piste %lu en lecture";
        static const char input_filepath[] = "[INPUT] chemin : %s";
        static const char input_raw_a_b[] = "[INPUT] BRUT A=%u B=%u";
        static const char input_raw_sw[] = "[INPUT] BRUT SW=%u";
        static const char input_clicky_value[] = "[INPUT] Valeur du potentiomètre à cliquet : %d";
        static const char input_volume[] = "[INPUT] Volume : %u";
        static const char input_clicky_zero[] = "[INPUT] Le potentiomètre à cliquet n'a pas tourné.";
        static const char input_switch_pressed[] = "[INPUT] L'interrupteur rotatif a été pressé.";
        static const char input_double_pressed[] = "[INPUT] Double pression, passage à la piste suivante.";
        static const char input_triple_pressed[] = "[INPUT] Triple pression, retour à la piste précédente.";
        static const char input_ultrasonic_pressed[] = "[INPUT] ultrason pressé.";
        static const char input_swipe_value[] = "[INPUT] Valeur du geste ultrason : %d";
        static const char input_swiped[] = "[INPUT] Geste ultrasonique détecté.";
        static const char input_track_finished[] = "[INPUT] Piste terminée, passage à la suivante.";

        // ─── Vogon panel task ─────────────────────────────────────────────────────
        static const char vogon_quote[] = "[VOGON] %s";
        static const char vogon_eom_enter[] = "[VOGON] Interlude de fin de poème";
        static const char vogon_stanza_switch[] = "[VOGON] Changement de strophe, prochaine strophe %u";
        static const char vogon_i2c_error[] = "[VOGON] Erreur d'écriture I2C %u (adresse 0x%02X)";

        // ─── Character LCD (info panel) ────────────────────────────────────────────
        static const char char_lcd_task_start[] = "[CHAR] Le panneau d'info tourne sur sa propre tâche.";
        static const char char_lcd_refresh[] = "[CHAR] Rafraîchissement : '%s'";
        static const char char_lcd_i2c_error[] = "[CHAR] Erreur d'écriture I2C %u (adresse 0x%02X)";
    } // namespace Infos
} // namespace My