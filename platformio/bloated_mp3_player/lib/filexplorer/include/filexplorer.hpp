#pragma once
#include <Arduino.h>
#include <SD.h>
#include <uicommon.hpp>

/**
 * @file filexplorer.hpp
 * @brief Directory-browser UI component rendered on a Canvas.
 *
 * Scans a directory, lists entries with a selection cursor, and
 * supports keyboard-style navigation (up/down/page/enter).
 */

/**
 * @brief  File / directory browser.
 *
 * Typical usage:
 * @code
 * FileExplorer fe;
 * fe.begin("/music");
 * // in input handler:
 * fe.down();
 * // in draw loop:
 * fe.draw(canvas, 0, 0, canvas.width(), canvas.height());
 * @endcode
 */
class FileExplorer
{
public:
    static constexpr uint16_t MAX_ENTRIES = 128;
    static constexpr uint16_t MAX_NAME    = 32;

    /** @brief A single directory entry. */
    struct Entry
    {
        char name[MAX_NAME];
        bool is_dir;
    };

    /**
     * @brief Initialise and scan a directory.
     * @param path Absolute path on the SD card (e.g. "/music").
     */
    bool begin(const char *path);

    /**
     * @brief Re-scan the current directory.
     * Call after enter_selected() changes the working directory.
     */
    bool scan(const char *path);

    /** @brief Switch to a new directory. */
    bool open_dir(const char *path);

    /**
     * @brief Enter the selected directory (if it is a directory).
     * @return false if selection is not a directory or navigation failed.
     */
    bool enter_selected();

    /** @brief Move cursor up one entry. */
    void up();

    /** @brief Move cursor down one entry. */
    void down();

    /** @brief Scroll up one page (~8 entries). */
    void page_up();

    /** @brief Scroll down one page (~8 entries). */
    void page_down();

    /** @brief Pointer to the currently selected Entry, or nullptr. */
    const Entry *selected() const;

    /** @brief Absolute path of the current directory. */
    const char *selected_path() const;

    /** @brief Number of entries in the current listing. */
    size_t count() const { return _count; }

    /** @brief Index of the selected entry. */
    size_t index() const { return _index; }

    /**
     * @brief Render the file list on a Canvas.
     *
     * Selected entry is highlighted with a white background.
     * Directories are drawn in cyan.
     *
     * @param canvas Target canvas.
     * @param x, y   Top-left corner of the list area.
     * @param w, h   Bounding box (clips overflow).
     */
    void draw(Canvas &canvas, int16_t x, int16_t y,
              uint16_t w, uint16_t h) const;

private:
    Entry _entries[MAX_ENTRIES];
    size_t _count = 0;
    size_t _index = 0;
    size_t _scroll = 0;
    char _current_dir[128];
};
