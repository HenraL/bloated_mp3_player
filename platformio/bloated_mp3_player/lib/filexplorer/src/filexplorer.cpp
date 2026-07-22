#include "filexplorer.hpp"

bool FileExplorer::begin(const char *path)
{
    strncpy(_current_dir, path, sizeof(_current_dir));
    return scan(_current_dir);
}

bool FileExplorer::scan(const char *path)
{
    File dir = SD_MMC.open(path);
    if (!dir || !dir.isDirectory())
        return false;

    _count = 0;
    File file;
    while ((file = dir.openNextFile()) && _count < MAX_ENTRIES) {
        Entry &e = _entries[_count];
        strncpy(e.name, file.name(), MAX_NAME - 1);
        e.name[MAX_NAME - 1] = '\0';
        e.is_dir = file.isDirectory();
        _count++;
        file.close();
    }
    dir.close();

    _index = 0;
    _scroll = 0;
    return true;
}

bool FileExplorer::open_dir(const char *path)
{
    strncpy(_current_dir, path, sizeof(_current_dir));
    return scan(_current_dir);
}

bool FileExplorer::enter_selected()
{
    if (_index >= _count) return false;
    const Entry &e = _entries[_index];
    if (!e.is_dir) return false;

    size_t len = strlen(_current_dir);
    if (len + 1 + strlen(e.name) + 1 > sizeof(_current_dir))
        return false;

    if (strcmp(e.name, "..") == 0) {
        char *slash = strrchr(_current_dir, '/');
        if (slash && slash != _current_dir)
            *slash = '\0';
        else if (slash)
            *(slash + 1) = '\0';
    } else {
        if (_current_dir[len - 1] != '/')
            strcat(_current_dir, "/");
        strcat(_current_dir, e.name);
    }
    return scan(_current_dir);
}

void FileExplorer::up()
{
    if (_index > 0) _index--;
    if (_index < _scroll) _scroll = _index;
}

void FileExplorer::down()
{
    if (_index < _count - 1) _index++;
}

void FileExplorer::page_up()
{
    int16_t page = 8;
    if (_index > (size_t)page) { _index -= page; } else { _index = 0; }
    if (_index < _scroll) _scroll = _index;
}

void FileExplorer::page_down()
{
    int16_t page = 8;
    if (_index + page < _count) { _index += page; } else { _index = _count - 1; }
}

const FileExplorer::Entry *FileExplorer::selected() const
{
    if (_index < _count) { return &_entries[_index]; }
    return nullptr;
}

const char *FileExplorer::selected_path() const
{
    return _current_dir;
}

void FileExplorer::draw(Canvas &canvas, int16_t x, int16_t y,
    uint16_t w, uint16_t h) const
{
    (void)w;
    int16_t line_h = 10;
    int16_t max_vis = h / line_h;
    int16_t cy = y;

    My::LED::Colour white(255, 255, 255, 0);
    My::LED::Colour black(0, 0, 0, 0);
    My::LED::Colour cyan(0, 255, 255, 0);

    for (size_t i = _scroll; i < _count && (i - _scroll) < (size_t)max_vis; i++) {
        const Entry &e = _entries[i];
        if (i == _index) {
            canvas.fill_rect(x, cy, canvas.width(), line_h, white);
            canvas.text(x + 2, cy + 1, e.name, black);
        } else {
            My::LED::Colour col;
            if (e.is_dir) { col = cyan; } else { col = white; }
            canvas.text(x + 2, cy + 1, e.name, col);
        }
        cy += line_h;
    }
}
