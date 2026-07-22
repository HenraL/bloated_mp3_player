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
* FILE: lcd.hpp
* CREATION DATE: 22-07-2026
* LAST Modified: 21:11:38 22-07-2026
* DESCRIPTION:
* This is the code in charge of making the bloated player come to life.
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: This is the overlay in charge of making the interraction with the lcd easier than interracting with the raw library.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <U8g2lib.h>
#include <uicommon.hpp>

namespace My
{
    namespace LCD
    {
        /**
         * @brief  Unified LCD display driver.
         *
         * Owns the U8G2 hardware instance and provides:
         *   - SPI initialisation and LCD reset
         *   - Screen-compatible drawing API (setFont, printAt, drawRect, …)
         *   - Canvas inheritance (pixel, clear, show, …)
         *
         * Use a single instance via SharedInstances::lcd.
         */
        class Display : public Canvas
        {
            public:
            Display();
            ~Display() = default;

            /** Initialise SPI bus and reset the LCD. */
            void initialise();

            // ── Lifecycle ───────────────────────────────────────────────
            /** Start the U8G2 driver and set default contrast. */
            void begin();
            void clear(My::LED::Colour c = My::LED::Colour(0, 0, 0, 0)) override;
            void show() override;
            void display() { show(); }

            // ── Font ─────────────────────────────────────────────────────
            void setFont(const uint8_t *font);
            void setFont(const BakedFonts::FontHandle *font);
            void setContrast(uint8_t value);

            // Canvas overrides for U8G2 rendering
            void text(int16_t x, int16_t y, const char *str,
                My::LED::Colour c) override;

            // ── U8G2-accelerated primitives ─────────────────────────────
            void drawPixel(uint16_t x, uint16_t y);
            void pixel(int16_t x, int16_t y, My::LED::Colour c) override;
            void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
            void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
            void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
            void drawCircle(uint16_t x0, uint16_t y0, uint16_t r);
            void drawHLine(uint16_t x, uint16_t y, uint16_t w);
            void drawVLine(uint16_t x, uint16_t y, uint16_t h);

            // ── Text ─────────────────────────────────────────────────────
            void setCursor(uint16_t x, uint16_t y);
            void print(const char *text);
            void printAt(const char *text, uint16_t x, uint16_t y);
            void printAt(uint16_t x, uint16_t y, const char *fmt, ...);

            // ── Images ───────────────────────────────────────────────────
            void drawAscii(const uint8_t *data, uint16_t w, uint16_t h, uint16_t x, uint16_t y);
            void draw_xbm(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint8_t *bits,
                My::LED::Colour fg = My::LED::Colour(255, 255, 255, 0)) override;
            void draw_image(int16_t x, int16_t y, const uint8_t *data,
                uint16_t w, uint16_t h,
                My::LED::Colour fg = My::LED::Colour(255, 255, 255, 0)) override;

            // ── Metrics ──────────────────────────────────────────────────
            uint16_t width() const override;
            uint16_t height() const override;

            private:
            U8G2_ST7565_ERC12864_ALT_F_4W_HW_SPI _u8g2;
            uint16_t _width = 128;
            uint16_t _height = 64;
            bool _initialised = false;
        };

    } // namespace LCD
} // namespace My
