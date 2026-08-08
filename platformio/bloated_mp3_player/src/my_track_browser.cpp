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
* FILE: track_browser.cpp
* CREATION DATE: 09-08-2026
* DESCRIPTION:
* Implementation of the folder-based selection state machine. The SD
* scan already groups tracks by folder (SDCard::get_folder), so here we
* only keep "which folder is highlighted" and "where inside it we are".
* /STOP
* COPYRIGHT: (c) Henry Letellier
* PURPOSE: Track/folder selection state machine implementation.
* // AR
* +==== END Bloated MP3 Player =================+
*/

#include "my/track_browser.hpp"

namespace My
{
    TrackBrowser::TrackBrowser()
        : _browsing(false)
        , _in_track_stage(false)
        , _folder_index(0)
        , _track_offset(0)
    {
    }

    void TrackBrowser::begin()
    {
        _browsing = true;
        _in_track_stage = false;
        _folder_index = 0;
        _track_offset = 0;
        if (folder_count() == 0) {
            _browsing = false;
        }
    }

    bool TrackBrowser::is_browsing() const
    {
        return _browsing;
    }

    bool TrackBrowser::in_folder_stage() const
    {
        return _browsing && !_in_track_stage;
    }

    bool TrackBrowser::in_track_stage() const
    {
        return _browsing && _in_track_stage;
    }

    void TrackBrowser::move_folder(int dir)
    {
        uint32_t n = folder_count();
        if (n == 0) {
            return;
        }
        if (dir > 0) {
            _folder_index = (_folder_index + 1) % n;
        } else if (dir < 0) {
            _folder_index = (_folder_index + n - 1) % n;
        }
        _track_offset = 0;
    }

    void TrackBrowser::enter_folder()
    {
        const SDCard::FolderInfo *fi = SDCard::get_folder(_folder_index);
        if (fi && fi->track_count > 0) {
            _in_track_stage = true;
            _track_offset = 0;
        }
    }

    void TrackBrowser::back_to_folders(void)
    {
        _in_track_stage = false;
        _track_offset = 0;
    }

    uint32_t TrackBrowser::folder_index() const
    {
        return _folder_index;
    }

    uint32_t TrackBrowser::folder_count() const
    {
        return SDCard::total_folders();
    }

    void TrackBrowser::move_track(int dir)
    {
        const SDCard::FolderInfo *fi = SDCard::get_folder(_folder_index);
        if (!fi || fi->track_count == 0) {
            return;
        }
        int32_t offset = (int32_t)_track_offset + dir;
        if (offset < 0) {
            if (_in_track_stage) {
                // Clamp at the start of the album while browsing tracks;
                // the user exits with the double-click, not by wrapping.
                _track_offset = 0;
                return;
            }
            // Back over the folder boundary: last track of the previous
            // folder (wrap around the folder table).
            uint32_t n = folder_count();
            _folder_index = (_folder_index + n - 1) % n;
            fi = SDCard::get_folder(_folder_index);
            if (fi) {
                _track_offset = fi->track_count - 1;
            }
        } else if (offset >= (int32_t)fi->track_count) {
            if (_in_track_stage) {
                // Clamp at the end of the album while browsing tracks.
                _track_offset = fi->track_count - 1;
                return;
            }
            // Forward over the boundary: first track of the next folder.
            uint32_t n = folder_count();
            _folder_index = (_folder_index + 1) % n;
            _track_offset = 0;
        } else {
            _track_offset = (uint32_t)offset;
        }
    }

    uint32_t TrackBrowser::track_index() const
    {
        const SDCard::FolderInfo *fi = SDCard::get_folder(_folder_index);
        if (!fi) {
            return 0;
        }
        return fi->first_track + _track_offset;
    }

    uint32_t TrackBrowser::folder_track_count() const
    {
        const SDCard::FolderInfo *fi = SDCard::get_folder(_folder_index);
        return fi ? fi->track_count : 0;
    }

    const char *TrackBrowser::track_path() const
    {
        const SDCard::TrackInfo *ti = SDCard::get_track(track_index());
        return ti ? ti->path : "";
    }

    const char *TrackBrowser::track_name() const
    {
        const SDCard::TrackInfo *ti = SDCard::get_track(track_index());
        return ti ? ti->filename : "";
    }

    const char *TrackBrowser::folder_name() const
    {
        const SDCard::FolderInfo *fi = SDCard::get_folder(_folder_index);
        return fi ? fi->folder : "";
    }

    void TrackBrowser::picked()
    {
        _browsing = false;
        _in_track_stage = false;
    }
} // namespace My