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
* FILE: debug.hpp
* CREATION DATE: 23-07-2026
* LAST Modified: 20:14:33 07-08-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the file in charge of providing the information wether to show a debug line or not.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
// Global debugging
#define ENABLE_GLOBAL_DEBUGGING
namespace My
{
    namespace Config
    {
        namespace Debug
        {
            // BMP280 debug lines
            static const bool UART_BMP280_RAW_MODULE_DATA = false;
            static const bool UART_BMP280_COMPUTED_MODULE_DATA = true;

            // Input debug lines
            // |- Clicky potentiometer
            static const bool UART_STICK_DIRECTION = false;
            static const bool UART_STICK_DIRECTION_ZERO = false;
            static const bool UART_STICK_TRACK_INDEX = false;
            static const bool UART_STICK_PRESSED = false;
            static const bool UART_STICK_DOUBLE_PRESSED = false;
            static const bool UART_STICK_TRIPLE_PRESSED = false;
            static const bool UART_STICK_VOLUME = true;
            static const bool UART_STICK_RAW = false;
            static const bool UART_STICK_RAW_SW = false;

            // When true (default), the rotary decoder uses the full A/B
            // quadrature state table; when false it falls back to the older
            // single-edge (A-only) decode. Compare on noisy wiring.
            static const bool ROTARY_QUADRATURE_DECODE = true;

            // When true, the pot direction is inverted. Use this if turning
            // the knob clockwise lowers the volume (A/B swapped or reversed
            // in your wiring) — set true and reflash instead of rewiring.
            static const bool ROTARY_DIRECTION_INVERTED = true;

            // ─── Serial profiling ─────────────────────────────────────────
            static const bool UART_PROFILING_ENABLED = false;

            // |- Ultrasonic
            static const bool UART_ULTRASONIC_PRESSED = false;
            static const bool UART_ULTRASONIC_SWIPED = false;
            static const bool UART_ULTRASONIC_SWIPE_VALUE = false;

            // SD Card
            static const bool UART_SD_TOTAL_TRACKS = true;
            static const bool UART_SD_MOUNTED = true;
            static const bool UART_SD_NO_TRACKS = true;
            static const bool UART_SD_ALL_TRACKS_PLAYED = true;
            static const bool UART_SD_CURRENT_TRACK_INDEX = true;
            static const bool UART_SD_AUDIO_PATH = true;

            // Sensor (tilt)
            static const bool UART_ANGLE_SENSOR_SHAKE = true;
            static const bool UART_ANGLE_SENSOR_TILT_LEFT = true;
            static const bool UART_ANGLE_SENSOR_TILT_RIGHT = true;

            // Audio
            static const bool UART_AUDIO_NOT_PLAYING = true;
            static const bool UART_AUDIO_NOT_LOADED = false;
            static const bool UART_AUDIO_TICKED = true;
            static const bool UART_AUDIO_STACK_HIGH_WATER = true;

            // When true, the audio task synthesises a 440Hz tone instead of
            // decoding tracks (used to test the I2S output stage in isolation).
            static const bool AUDIO_TEST_TONE_ENABLED = false;

            // Test-tone peak amplitude (0..32767). Keep well below full
            // scale: the RC-filtered PWM drives the LM386 modules directly,
            // and full scale saturates them (duty range 0..255 = 0..3.3V).
            static const int16_t AUDIO_TEST_TONE_AMPLITUDE = 8192;

            // LED stack usage
            static const bool UART_LED_STACK_HIGH_WATER = true;

            // When true, the UI task draws a single static frame and then
            // stops repainting. Used to distinguish display flicker caused by
            // code (continuous refresh) from flicker caused by wiring.
            static const bool LCD_ONESHOT_TEST_ENABLED = true;

            // Character LCD (PCF8574 info panel) debug lines
            static const bool UART_CHAR_LCD_REFRESH = false;
            static const bool UART_CHAR_LCD_I2C_ERROR = true;

            // Vogon panel (second character LCD) debug lines
            static const bool UART_VOGON_STANZA_SWITCH = false;
            static const bool UART_VOGON_EOM_ENTER = false;
            static const bool UART_VOGON_I2C_ERROR = true;

        } // namespace Debug

    } // namespace Config

} // namespace My
