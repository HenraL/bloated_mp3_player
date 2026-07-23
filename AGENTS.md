<!-- 
-- +==== BEGIN Bloated MP3 Player =================+
-- LOGO:
-- .......................
-- ...><>.............<><.
-- ..><>.><>.......<><.<><
-- .><>.<><.><>.<><.<><.<>
-- ..><>.><>.......<><.<><
-- ...><>.............<><.
-- .......................
-- /STOP
-- PROJECT: Bloated MP3 Player
-- FILE: AGENTS.md
-- CREATION DATE: 23-07-2026
-- LAST Modified: 4:7:8 23-07-2026
-- DESCRIPTION:
-- This is the code in charge of making the bloated player come to life.
-- /STOP
-- COPYRIGHT: (c) Henry Letellier
-- PURPOSE: This is a document to help ai agents respect the infrastructure to some extent when providing help in the program.
-- // AR
-- +==== END Bloated MP3 Player =================+
-->
# AGENTS.md — Norms & Conventions for AI Assistants

This file documents project-specific conventions, rules, and non-obvious gotchas
that AI coding assistants must follow when working on this codebase.

---

## Golden Rules

### 1. Always use Python virtual environments

Never run Python scripts with system Python. Every Python tool has a dedicated venv:

| Tool | Script | Venv (auto-created by script) | Requirements |
|------|--------|-------------------------------|--------------|
| Font → C converter | `bonus/font2c.sh` | `bonus/font2c_env/` | `bonus/font2c/requirements.txt` |
| Image → C converter | `bonus/img2c.sh` | `bonus/img2c_env/` | `bonus/img2c/requirements.txt` |

The shell scripts create their own venv from `requirements.txt` and handle
activation/deactivation automatically. If running `convert.py` directly, create
and activate a venv first:

```bash
python3 -m venv .venv
. .venv/bin/activate
pip install -r bonus/font2c/requirements.txt
python bonus/font2c/convert.py ...
deactivate
```

No venvs are shipped with the repository — they are generated from
`requirements.txt` by the scripts or manually.

### 2. Never hand-edit generated files

Files under `lib/fonts/` and `lib/images/` are **generated** by the Python converters.
Always modify the converter (`bonus/font2c/convert.py` or `bonus/img2c/convert.py`) and re-run.
Hand-edits will be overwritten.

### 3. `using namespace` is banned

Every type must be accessed via its full qualified name. No `using namespace std;`,
no `using namespace BakedFonts;`, etc. Exception: `using` for specific types in narrow scopes
(e.g. `using My::Config::Structures::duration_ms_t`) is acceptable.

### 4. Ternary `? :` with brace bodies is banned

Only `if`/`else` blocks. Simple value-expression ternaries (e.g. `int x = cond ? a : b;`) are fine.

### 5. No `static` variables in .cpp files

All state must live in class member fields. File-scope `static` variables are not allowed.

### 6. Debug serial uses `SharedInstances::serial.serial_print()`

Never `Serial.print()`. Always go through the shared serial instance.

### 7. `constexpr` is banned

Use `static const` inside `namespace` instead. `constexpr` can trigger undefined behaviour on
certain embedded compilers (ESP8266, some GCC/architecture combinations).  Use `static const`
at namespace scope — it's safe and gives the same compile-time constant semantics.

### 8. Never execute git commands

AI assistants must **never** run `git add`, `git commit`, `git push`, `git submodule`, `git tag`, or any
other command that alters repository state. Only read-only inspection (`git status`, `git diff`,
`git log`, `git show`) is permitted. The user handles all version control operations manually.

---

## Coding Conventions

### Naming

| Scope | Style | Example |
|-------|-------|---------|
| Config/pin constants, enums, macros | `UPPER_SNAKE_CASE` | `LCD_CS`, `STYLE_NORMAL` |
| Namespaces, classes, structs, enum types | `PascalCase` | `My::LCD::Display`, `BakedFonts::FontHandle` |
| Functions, methods, variables | `snake_case` | `set_baked_font()`, `fill_rect()` |
| Private member fields | Leading underscore | `_u8g2`, `_baked_font` |
| Font handle variables | `snake_case` + `_handle` suffix | `tiny5_8pt_handle` |
| Font bit data arrays | `snake_case` + `_bits` suffix | `tiny5_8pt_regular_bits` |

### Formatting

- **Indentation**: 4 spaces, no tabs
- **Braces — namespaces/classes/functions**: Allman (own line)
- **Braces — control flow** (`if`/`for`/`while`): K&R (same line)
- **Include guards**: `#pragma once` everywhere
- **File headers**: ASCII-art banner block (see existing files for template)

### Namespace layout

- Application code: `My::` → `My::Config::`, `My::LCD::`, `My::LED::`, etc.
- Font library: `BakedFonts::` → `BakedFonts::Size::`, `BakedFonts::Family::`, `BakedFonts::Helpers::`
- Library code: top-level namespaces like `Screen`, `Matrix`, `FileExplorer`
- Namespace closing comments required: `} // namespace Foo`

### PROGMEM

Large read-only data (font bitmaps, lookup tables, message sequences) must use `PROGMEM`.
Read back with `pgm_read_byte()`, `pgm_read_ptr()`, or `memcpy_P()`.

### File naming

- Headers: `.hpp`
- Sources: `.cpp`
- Config headers: `include/my/config/<name>.hpp`

---

## Project Structure

```
platformio/bloated_mp3_player/
├── bonus/
│   ├── font2c.sh          # Font → C conversion wrapper (handles venv)
│   ├── img2c.sh           # Image → C conversion wrapper (handles venv)
│   ├── font2c/convert.py  # Font converter
│   ├── img2c/convert.py   # Image converter
│   ├── fonts/             # Source TTF/OTF files
│   ├── images/            # Source PNG files
│   ├── font2c_env/        # Font2c venv (auto-created by font2c.sh)
│   └── extras/            # Flamegraph profiler, etc.
├── include/               # Application headers
│   └── my/
│       ├── config/        # Pinout, layout, delays, priorities, etc.
│       ├── lcd.hpp        # Display driver wrapper
│       └── ...
├── lib/                   # Library modules (each with include/ + src/)
│   ├── fonts/             # Generated font C library
│   ├── images/            # Generated image C library
│   ├── uicommon/          # Canvas base class, text rendering
│   └── ...
├── src/                   # Application source
└── platformio.ini
```

---

## Font System (BakedFonts)

Key types in `lib/fonts/include/`:

- `FontHandle` — struct with `width`, `height`, `ascent`, `count`, `codes`, `widths`, `bits`
- `RenderedGlyph` — per-glyph data (width, height, offset_x, offset_y, bits pointer)

The `Canvas::text()` method in `lib/uicommon/src/uicommon.cpp` handles rendering:

- `y` parameter is **baseline** (matching U8g2 convention)
- `cell_top = y - fh->ascent` converts to top-of-cell
- Space glyph (U+0020) has width=0 in the font data; renderer uses `glyph_width / 3 + 1` as fallback
- UTF-8 decoding via `utf8_decode()` static helper

U8g2 `setCursor(x, y)` also treats y as baseline. When switching between BakedFonts and U8g2 fonts,
Y coordinates should be adjusted by `ascent_offset`.

---

## Key "Don't Touch" Files

- `lib/fonts/` (entire directory) — regenerated by `font2c.sh`
- `lib/images/` (entire directory) — regenerated by `img2c.sh`

---

## When in doubt

- Check existing code for patterns before writing new code
- This file was written precisely to answer these questions — read it first
