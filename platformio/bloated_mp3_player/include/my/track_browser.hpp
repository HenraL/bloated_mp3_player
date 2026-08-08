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
* FILE: track_browser.hpp
* CREATION DATE: 09-08-2026
* DESCRIPTION:
* Owns the "which music do we play" decision. Two modes: browsing
* (the user walks the folder list with the rotary knob) and playing
* (a playback position inside a folder, ready to step to the next
* audio of that folder or roll into the next folder).
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Track/folder selection state machine.
* // AR
* +==== END Bloated MP3 Player =================+
*/
#pragma once
#include <stdint.h>
#include <sdcard.hpp>

namespace My
{
    class TrackBrowser
    {
        public:
        TrackBrowser();

        // ─── Mode ──────────────────────────────────────────────────────────
        void begin();                      // reset to browsing, folder 0
        bool is_browsing() const;

        // ─── Browsing ──────────────────────────────────────────────────────
        void move_folder(int dir);         // walk the folder table (wrap)
        uint32_t folder_index() const;
        uint32_t folder_count() const;

        // ─── Positioned track ──────────────────────────────────────────────
        void move_track(int dir);          // step inside folder (wrap folder)
        uint32_t track_index() const;      // global track list index
        uint32_t folder_track_count() const;

        // Info of the track behind the current position.
        const char *track_path() const;
        const char *track_name() const;
        const char *folder_name() const;   // the album the track sits in

        // Confirm the browser choice: play first audio of the folder.
        void picked();

        private:
        bool     _browsing;
        uint32_t _folder_index;
        uint32_t _track_offset;
    };
} // namespace My