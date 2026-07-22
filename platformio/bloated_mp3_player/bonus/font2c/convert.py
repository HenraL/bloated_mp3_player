""" 
# +==== BEGIN Bloated MP3 Player =================+
# LOGO:
# .......................
# ...><>.............<><.
# ..><>.><>.......<><.<><
# .><>.<><.><>.<><.<><.<>
# ..><>.><>.......<><.<><
# ...><>.............<><.
# .......................
# /STOP
# PROJECT: Bloated MP3 Player
# FILE: convert.py
# CREATION DATE: 16-07-2026
# LAST Modified: 20:0:57 22-07-2026
# DESCRIPTION:
# Convert TTF / OTF font files to C byte arrays for embedded displays.
#
# Uses fonttools to query each font's character map (cmap) so only
# code points the font actually supports are rendered — no brute-force.
#
# Output is compact: only glyphs the font supports are stored (no
# padding for missing code points).  A codes[] array maps each glyph
# index to its Unicode code point for binary-search lookup.
#
# In batch mode, fonts belonging to the same family (e.g. Roboto,
# Roboto-Bold, Roboto-Italic) are merged into a single output entry
# with a FontStyleMap cross-referencing code points across variants.
#
# Output structure:
#   include/         fonts.hpp, font_structs.hpp, font_constants.hpp,
#                    font_families.hpp, font_helpers.hpp
#   include/internal/<family>/   per-size headers
#   src/             per-size .cpp files
#   library.json
#
# Usage:
#   python convert.py Roboto-Regular.ttf output_lib/ --size 12
#   python convert.py input_font_dir/ output_lib/ --batch --size 14
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: Generate C headers and source files for TTF/OTF fonts.
# // AR
# +==== END Bloated MP3 Player =================+
"""

import os
import sys
import json
import math
import argparse
import typing
import time
from datetime import datetime, timezone
from dataclasses import dataclass

from PIL import ImageFont
from fontTools.ttLib import TTFont


# ─── Constants ────────────────────────────────────────────────────────────

FILE_ENCODING: str = "utf-8"

STYLE_NORMAL: int = 0
STYLE_ITALIC: int = 1
STYLE_BOLD: int = 2
STYLE_UNDERLINE: int = 3
STYLE_STRIKETHROUGH: int = 4
STYLE_COUNT: int = 5

STYLE_VARIANT_ABSENT: int = 0x0000

VARIANT_RULES: typing.List[typing.Tuple[str, str]] = [
    ("bolditalic", "-BoldItalic"),
    ("boldoblique", "-BoldOblique"),
    ("italic", "-Italic"),
    ("oblique", "-Oblique"),
    ("bold", "-Bold"),
    ("regular", "-Regular"),
    ("roman", "-Roman"),
    ("medium", "-Medium"),
    ("light", "-Light"),
    ("thin", "-Thin"),
    ("black", "-Black"),
    ("semibold", "-SemiBold"),
    ("extrabold", "-ExtraBold"),
    ("extralight", "-ExtraLight"),
]

VARIANT_TO_STYLE_INDEX: typing.Dict[str, int] = {
    "normal": STYLE_NORMAL,
    "regular": STYLE_NORMAL,
    "roman": STYLE_NORMAL,
    "medium": STYLE_NORMAL,
    "light": STYLE_NORMAL,
    "thin": STYLE_NORMAL,
    "extralight": STYLE_NORMAL,
    "italic": STYLE_ITALIC,
    "oblique": STYLE_ITALIC,
    "bold": STYLE_BOLD,
    "bolditalic": STYLE_BOLD,
    "boldoblique": STYLE_BOLD,
    "semibold": STYLE_BOLD,
    "extrabold": STYLE_BOLD,
    "black": STYLE_BOLD,
}


# ─── Data classes ─────────────────────────────────────────────────────────

@dataclass
class RenderedGlyph:
    code: int
    width: int
    height: int
    pixels: typing.List[int]


@dataclass
class RenderedSize:
    point_size: int
    cell_height: int
    cell_width: int
    glyphs: typing.Dict[int, RenderedGlyph]


# ─── File name utilities ─────────────────────────────────────────────────

def sanitise_name(raw: str) -> str:
    base: str = os.path.splitext(os.path.basename(raw))[0]
    clean: typing.List[str] = []
    for ch in base:
        if ch.isalnum() or ch == '_':
            clean.append(ch)
        else:
            clean.append('_')
    name: str = ''.join(clean)
    if name and name[0].isdigit():
        name = '_' + name
    if not name:
        name = "font"
    return name.lower()


def parse_variant(filename: str) -> typing.Tuple[str, str]:
    name: str = os.path.splitext(os.path.basename(filename))[0]
    lower: str = name.lower()
    vf_idx: int = lower.find("-variablefont")
    if vf_idx != -1:
        clean: str = name[:vf_idx]
        clean_lower: str = clean.lower()
        for variant, suffix in VARIANT_RULES:
            if clean_lower.endswith(suffix.lower()):
                base: str = clean[:-len(suffix)]
                return base, "normal"
        return clean, "normal"
    for variant, suffix in VARIANT_RULES:
        if lower.endswith(suffix.lower()):
            base: str = name[:-len(suffix)]
            return base, variant
    return name, "normal"


def is_variable_font(font_path: str) -> bool:
    try:
        font: TTFont = TTFont(font_path, fontNumber=0)
        result: bool = "fvar" in font
        font.close()
        return result
    except Exception:
        return False


# ─── Cmap extraction ─────────────────────────────────────────────────────

def extract_cmap(font_path: str) -> typing.Set[int]:
    font: TTFont = TTFont(font_path, fontNumber=0)
    try:
        cmap: typing.Dict[int, str] = font.getBestCmap()
        if cmap is None:
            return set()
        return set(cmap.keys())
    finally:
        font.close()


# ─── Rendering ────────────────────────────────────────────────────────────

def render_ttf_glyph(
    font: ImageFont.FreeTypeFont,
    code: int,
) -> typing.Optional[RenderedGlyph]:
    char: str = chr(code)
    try:
        result: typing.Any = font.getmask2(char, mode="1")
    except (ValueError, OSError):
        return None
    if result is None:
        return None
    mask: typing.Any
    offset: typing.Tuple[int, int]
    mask, offset = result
    mask_w: int
    mask_h: int
    mask_w, mask_h = mask.size
    if mask_w == 0 or mask_h == 0:
        return None
    flat: typing.List[int] = list(mask)
    row_bytes: int = (mask_w + 7) // 8
    pixels: typing.List[int] = [0] * (row_bytes * mask_h)
    for y in range(mask_h):
        for x in range(mask_w):
            if flat[y * mask_w + x]:
                byte_idx: int = y * row_bytes + x // 8
                pixels[byte_idx] |= 1 << (7 - (x % 8))
    return RenderedGlyph(
        code=code,
        width=mask_w,
        height=mask_h,
        pixels=pixels,
    )


def render_size(
    font_path: str,
    point_size: int,
    codes_to_render: typing.List[int],
    progress_callback: typing.Optional[typing.Callable[[int, int], None]] = None,
) -> RenderedSize:
    font: ImageFont.FreeTypeFont = ImageFont.truetype(font_path, point_size)
    glyphs: typing.Dict[int, RenderedGlyph] = {}
    max_w: int = 0
    max_h: int = 0
    total: int = len(codes_to_render)
    for idx, code in enumerate(codes_to_render):
        if progress_callback is not None:
            progress_callback(idx, total)
        glyph: typing.Optional[RenderedGlyph] = render_ttf_glyph(font, code)
        if glyph is not None:
            glyphs[code] = glyph
            if glyph.width > max_w:
                max_w = glyph.width
            if glyph.height > max_h:
                max_h = glyph.height
    if max_h == 0:
        max_h = point_size
    return RenderedSize(
        point_size=point_size,
        cell_height=max_h,
        cell_width=max_w,
        glyphs=glyphs,
    )


# ─── Array building ──────────────────────────────────────────────────────

def build_bits_array(
    size_data: RenderedSize,
    codes: typing.List[int],
) -> typing.Tuple[typing.List[int], typing.List[int]]:
    cell_h: int = size_data.cell_height
    bits: typing.List[int] = []
    widths: typing.List[int] = []
    for code in codes:
        glyph: typing.Optional[RenderedGlyph] = size_data.glyphs.get(code)
        if glyph is None:
            widths.append(0)
            continue
        widths.append(glyph.width)
        glyph_row_bytes: int = (glyph.width + 7) // 8
        for row in range(cell_h):
            if row < glyph.height:
                start: int = row * glyph_row_bytes
                end: int = start + glyph_row_bytes
                bits.extend(glyph.pixels[start:end])
            else:
                bits.extend([0] * glyph_row_bytes)
    return bits, widths


def build_style_map(
    variants_data: typing.Dict[str, RenderedSize],
    codes: typing.List[int],
) -> typing.List[typing.List[int]]:
    style_map: typing.List[typing.List[int]] = []
    for code in codes:
        entry: typing.List[int] = [STYLE_VARIANT_ABSENT] * STYLE_COUNT
        for variant_name, size_data in variants_data.items():
            if code in size_data.glyphs:
                idx: int = VARIANT_TO_STYLE_INDEX.get(variant_name.lower())
                if idx is not None:
                    entry[idx] = code
        style_map.append(entry)
    return style_map


# ─── C array formatting ─────────────────────────────────────────────────

def format_c_array(
    data: typing.List[int],
    indent: str = "    ",
    width: int = 16,
) -> typing.List[str]:
    lines: typing.List[str] = []
    for chunk_start in range(0, len(data), width):
        chunk: typing.List[int] = data[chunk_start:chunk_start + width]
        parts: typing.List[str] = [f"0x{b:02x}" for b in chunk]
        line: str = ", ".join(parts)
        is_last: bool = chunk_start + width >= len(data)
        if is_last:
            lines.append(f"{indent}{line}")
        else:
            lines.append(f"{indent}{line},")
    return lines


def format_codes_array(
    data: typing.List[int],
    indent: str = "    ",
    width: int = 8,
) -> typing.Tuple[str, typing.List[str]]:
    is_wide: bool = any(c > 0xFFFF for c in data)
    ctype: str = "uint32_t" if is_wide else "uint16_t"
    fmt: str = "0x{v:08x}" if is_wide else "0x{v:04x}"
    lines: typing.List[str] = []
    for chunk_start in range(0, len(data), width):
        chunk: typing.List[int] = data[chunk_start:chunk_start + width]
        parts: typing.List[str] = [fmt.format(v=v) for v in chunk]
        line: str = ", ".join(parts)
        is_last: bool = chunk_start + width >= len(data)
        if is_last:
            lines.append(f"{indent}{line}")
        else:
            lines.append(f"{indent}{line},")
    return ctype, lines


def format_style_map_array(
    style_map: typing.List[typing.List[int]],
    indent: str = "    ",
) -> typing.List[str]:
    lines: typing.List[str] = []
    for entry in style_map:
        parts: typing.List[str] = [f"0x{v:04x}" for v in entry]
        lines.append(f"{indent}{{ {', '.join(parts)} }},")
    return lines


# ─── Size info container ─────────────────────────────────────────────────

@dataclass
class SizeOutputInfo:
    pt: int
    cell_width: int
    cell_height: int
    codes: typing.List[int]
    variant_data: typing.Dict[str, typing.Tuple[typing.List[int], typing.List[int]]]
    style_map: typing.List[typing.List[int]]
    variant_names: typing.List[str]


# ─── Library file generation ─────────────────────────────────────────────

def _write_family_size_header_and_source(
    lib_dir: str,
    family_name: str,
    info: SizeOutputInfo,
    use_extern: bool,
    use_progmem: bool,
) -> str:
    pt: int = info.pt
    size_tag: str = f"{pt}pt"
    name_tag: str = f"{family_name}_{size_tag}"

    include_dir: str = os.path.join(lib_dir, "include", "internal", family_name)
    source_dir: str = os.path.join(lib_dir, "src")
    os.makedirs(include_dir, exist_ok=True)
    os.makedirs(source_dir, exist_ok=True)

    progmem_attr: str = " PROGMEM" if use_progmem else ""
    first_code: int = info.codes[0]
    last_code: int = info.codes[-1]
    code_count: int = len(info.codes)

    array_names: typing.List[typing.Tuple[str, str, typing.List[int], typing.List[int]]] = []
    for vname in info.variant_names:
        bits, widths = info.variant_data[vname]
        if vname == "normal":
            bits_name: str = f"{name_tag}_bits"
            widths_name: str = f"{name_tag}_widths"
        else:
            bits_name = f"{name_tag}_{vname}_bits"
            widths_name = f"{name_tag}_{vname}_widths"
        array_names.append((bits_name, widths_name, bits, widths))

    has_style_map: bool = bool(info.style_map) and len(info.variant_names) > 1
    arduino_include: str = '#include <Arduino.h>\n' if use_progmem else ''

    # ── Build header ──
    guard_upper: str = name_tag.upper().replace("-", "_")
    hdr: str = ""
    hdr += f"#ifndef BAKEDFONTS_{guard_upper}_H_\n"
    hdr += f"#define BAKEDFONTS_{guard_upper}_H_\n"
    hdr += "\n"
    hdr += "#include <stdint.h>\n"
    hdr += arduino_include
    hdr += '#include "font_structs.hpp"\n'
    hdr += "\n"
    hdr += "namespace BakedFonts {\n"
    hdr += "\n"
    hdr += f"#define {name_tag.upper()}_WIDTH  {info.cell_width}\n"
    hdr += f"#define {name_tag.upper()}_HEIGHT {info.cell_height}\n"
    hdr += f"#define {name_tag.upper()}_FIRST  {first_code}\n"
    hdr += f"#define {name_tag.upper()}_LAST   {last_code}\n"
    hdr += f"#define {name_tag.upper()}_COUNT  {code_count}\n"
    hdr += "\n"

    # codes[] array — maps index to Unicode code point
    codes_ctype: str
    codes_lines: typing.List[str]
    codes_ctype, codes_lines = format_codes_array(info.codes)
    codes_type_decl: str = f"const {codes_ctype} {name_tag}_codes[]"
    if use_extern:
        hdr += f"extern {codes_type_decl};\n"
    else:
        hdr += f"static {codes_type_decl} = {{\n"
        for line in codes_lines:
            hdr += line + "\n"
        hdr += "};\n"
    hdr += "\n"

    for bits_name, widths_name, bits_data, widths_data in array_names:
        if use_extern:
            hdr += f"extern const uint8_t {bits_name}[]{progmem_attr};\n"
            hdr += f"extern const uint8_t {widths_name}[];\n"
        else:
            hdr += f"static const uint8_t {bits_name}[]{progmem_attr} = {{\n"
            for line in format_c_array(bits_data):
                hdr += line + "\n"
            hdr += "};\n"
            hdr += "\n"
            hdr += f"static const uint8_t {widths_name}[] = {{\n"
            for line in format_c_array(widths_data):
                hdr += line + "\n"
            hdr += "};\n"

    if has_style_map:
        if use_extern:
            hdr += f"extern const FontStyleMap {name_tag}_style_map[];\n"
        else:
            hdr += f"static const FontStyleMap {name_tag}_style_map[] = {{\n"
            for line in format_style_map_array(info.style_map):
                hdr += line + "\n"
            hdr += "};\n"

    hdr += "\n"
    hdr += "}  // namespace BakedFonts\n"
    hdr += "\n"
    hdr += f"#endif /* BAKEDFONTS_{guard_upper}_H_ */\n"

    header_path: str = os.path.join(include_dir, f"{family_name}_{size_tag}.hpp")
    with open(header_path, "w", encoding=FILE_ENCODING) as handle:
        handle.write(hdr)
    print(f"  [ OK ] internal/{family_name}/{family_name}_{size_tag}.hpp ({code_count} chars)")

    # ── Source file (extern mode only) ──
    if use_extern:
        src: str = ""
        src += '#include <Arduino.h>\n'
        src += f'#include "internal/{family_name}/{family_name}_{size_tag}.hpp"\n'
        src += "\n"
        src += "namespace BakedFonts {\n"
        src += "\n"
        codes_ctype, codes_lines = format_codes_array(info.codes)
        src += f"const {codes_ctype} {name_tag}_codes[] = {{\n"
        for line in codes_lines:
            src += line + "\n"
        src += "};\n"
        src += "\n"
        for bits_name, widths_name, bits_data, widths_data in array_names:
            src += f"const uint8_t {bits_name}[]{progmem_attr} = {{\n"
            for line in format_c_array(bits_data):
                src += line + "\n"
            src += "};\n"
            src += "\n"
            src += f"const uint8_t {widths_name}[] = {{\n"
            for line in format_c_array(widths_data):
                src += line + "\n"
            src += "};\n"
            src += "\n"
        if has_style_map:
            src += f"const FontStyleMap {name_tag}_style_map[] = {{\n"
            for line in format_style_map_array(info.style_map):
                src += line + "\n"
            src += "};\n"
            src += "\n"
        src += "}  // namespace BakedFonts\n"
        src += "\n"
        source_path: str = os.path.join(source_dir, f"{family_name}_{size_tag}.cpp")
        with open(source_path, "w", encoding=FILE_ENCODING) as handle:
            handle.write(src)
        print(f"  [ OK ] src/{family_name}_{size_tag}.cpp")

    return size_tag


def _write_family_aggregator(
    lib_dir: str,
    family_name: str,
    size_tags: typing.List[str],
) -> None:
    guard_upper: str = family_name.upper().replace("-", "_")
    content: str = ""
    content += f"#ifndef BAKEDFONTS_{guard_upper}_H_\n"
    content += f"#define BAKEDFONTS_{guard_upper}_H_\n"
    content += "\n"
    for tag in size_tags:
        content += f'#include "{family_name}/{family_name}_{tag}.hpp"\n'
    content += "\n"
    content += f"#endif /* BAKEDFONTS_{guard_upper}_H_ */\n"
    path: str = os.path.join(lib_dir, "include", "internal", f"{family_name}.hpp")
    with open(path, "w", encoding=FILE_ENCODING) as handle:
        handle.write(content)
    print(f"[ OK ] include/internal/{family_name}.hpp")


def write_library_metadata(
    lib_dir: str,
    all_sizes: typing.List[int],
    families: typing.List[typing.Tuple[str, int]],
) -> None:
    include_dir: str = os.path.join(lib_dir, "include")
    os.makedirs(include_dir, exist_ok=True)

    # ── font_structs.hpp ──
    structs: str = ""
    structs += "#pragma once\n"
    structs += "\n"
    structs += "#include <stdint.h>\n"
    structs += "\n"
    structs += "namespace BakedFonts {\n"
    structs += "\n"
    structs += "struct FontStyleMap {\n"
    structs += "    uint16_t normal;\n"
    structs += "    uint16_t italic;\n"
    structs += "    uint16_t bold;\n"
    structs += "    uint16_t underline;\n"
    structs += "    uint16_t strikethrough;\n"
    structs += "};\n"
    structs += "\n"
    structs += "struct FontHandle {\n"
    structs += "    const uint16_t *codes;\n"
    structs += "    const uint8_t *bits;\n"
    structs += "    const uint8_t *widths;\n"
    structs += "    uint16_t count;\n"
    structs += "    uint8_t glyph_width;\n"
    structs += "    uint8_t glyph_height;\n"
    structs += "};\n"
    structs += "\n"
    structs += "}\n"
    with open(os.path.join(include_dir, "font_structs.hpp"), "w", encoding=FILE_ENCODING) as f:
        f.write(structs)
    print("[ OK ] include/font_structs.hpp")

    # ── font_constants.hpp ──
    size_lines: typing.List[str] = []
    for pt in sorted(set(all_sizes)):
        size_lines.append(f"    static constexpr uint8_t PT_{pt} = {pt};")

    family_name_lines: typing.List[str] = []
    for name, _ in sorted(set(families), key=lambda x: x[0]):
        c_name: str = name.upper()
        family_name_lines.append(f'    static constexpr char {c_name}[] = "{name}";')

    consts: str = ""
    consts += "#pragma once\n"
    consts += "\n"
    consts += "#include <stdint.h>\n"
    consts += "\n"
    consts += "namespace BakedFonts {\n"
    consts += "\n"
    consts += "static constexpr uint16_t STYLE_VARIANT_ABSENT = 0x0000;\n"
    consts += "\n"
    consts += "enum StyleIndex : uint8_t {\n"
    consts += "    STYLE_NORMAL = 0,\n"
    consts += "    STYLE_ITALIC = 1,\n"
    consts += "    STYLE_BOLD = 2,\n"
    consts += "    STYLE_UNDERLINE = 3,\n"
    consts += "    STYLE_STRIKETHROUGH = 4,\n"
    consts += "    STYLE_COUNT = 5\n"
    consts += "};\n"
    consts += "\n"
    consts += "namespace Size {\n"
    for line in size_lines:
        consts += line + "\n"
    consts += "}\n"
    consts += "\n"
    consts += "namespace Family {\n"
    for line in family_name_lines:
        consts += line + "\n"
    consts += "}\n"
    consts += "\n"
    consts += "}\n"
    with open(os.path.join(include_dir, "font_constants.hpp"), "w", encoding=FILE_ENCODING) as f:
        f.write(consts)
    print("[ OK ] include/font_constants.hpp")

    # ── font_families.hpp ──
    avail_lines: typing.List[str] = []
    for name, size_count in sorted(families, key=lambda x: x[0]):
        c_name: str = name.upper()
        avail_lines.append(f"    {{ {c_name}, {size_count} }},")

    fams: str = ""
    fams += "#pragma once\n"
    fams += "\n"
    fams += "#include <stdint.h>\n"
    fams += "\n"
    fams += "namespace BakedFonts {\nnamespace Family {\n"
    fams += "\n"
    fams += "struct Entry {\n"
    fams += "    const char* name;\n"
    fams += "    uint8_t size_count;\n"
    fams += "};\n"
    fams += "\n"
    fams += "static constexpr Entry AVAILABLE[] = {\n"
    for line in avail_lines:
        fams += line + "\n"
    fams += "};\n"
    fams += "\n"
    fams += f"static constexpr uint8_t COUNT = {len(families)};\n"
    fams += "\n"
    fams += "}\n}\n"
    with open(os.path.join(include_dir, "font_families.hpp"), "w", encoding=FILE_ENCODING) as f:
        f.write(fams)
    print("[ OK ] include/font_families.hpp")

    # ── font_helpers.hpp (binary-search-based lookup) ──
    helpers: str = ""
    helpers += "#pragma once\n"
    helpers += "\n"
    helpers += '#include "font_structs.hpp"\n'
    helpers += '#include "font_constants.hpp"\n'
    helpers += "\n"
    helpers += "#include <stdint.h>\n"
    helpers += "\n"
    helpers += "namespace BakedFonts { namespace Helpers {\n"
    helpers += "\n"
    helpers += "inline uint16_t glyph_index(\n"
    helpers += "    const uint32_t codes[], uint16_t count, uint32_t code_point)\n"
    helpers += "{\n"
    helpers += "    int16_t lo = 0;\n"
    helpers += "    int16_t hi = (int16_t)(count - 1);\n"
    helpers += "    while (lo <= hi) {\n"
    helpers += "        uint16_t mid = (uint16_t)((lo + hi) / 2);\n"
    helpers += "        if (codes[mid] == code_point) return mid;\n"
    helpers += "        if (codes[mid] < code_point) lo = (int16_t)(mid + 1);\n"
    helpers += "        else hi = (int16_t)(mid - 1);\n"
    helpers += "    }\n"
    helpers += "    return 0xFFFF;\n"
    helpers += "}\n"
    helpers += "\n"
    helpers += "inline bool glyph_available(\n"
    helpers += "    const uint32_t codes[], uint16_t count,\n"
    helpers += "    const uint8_t widths[], uint32_t code_point)\n"
    helpers += "{\n"
    helpers += "    uint16_t idx = glyph_index(codes, count, code_point);\n"
    helpers += "    if (idx == 0xFFFF) return false;\n"
    helpers += "    return widths[idx] != 0;\n"
    helpers += "}\n"
    helpers += "\n"
    helpers += "inline uint8_t glyph_width(\n"
    helpers += "    const uint32_t codes[], uint16_t count,\n"
    helpers += "    const uint8_t widths[], uint32_t code_point)\n"
    helpers += "{\n"
    helpers += "    uint16_t idx = glyph_index(codes, count, code_point);\n"
    helpers += "    if (idx == 0xFFFF) return 0;\n"
    helpers += "    return widths[idx];\n"
    helpers += "}\n"
    helpers += "\n"
    helpers += "inline uint16_t style_variant(\n"
    helpers += "    const uint32_t codes[], uint16_t count,\n"
    helpers += "    const FontStyleMap map[],\n"
    helpers += "    uint32_t code_point, StyleIndex style)\n"
    helpers += "{\n"
    helpers += "    uint16_t idx = glyph_index(codes, count, code_point);\n"
    helpers += "    if (idx == 0xFFFF) return STYLE_VARIANT_ABSENT;\n"
    helpers += "    if (style >= STYLE_COUNT) return STYLE_VARIANT_ABSENT;\n"
    helpers += "    switch (style) {\n"
    helpers += "        case STYLE_NORMAL: return map[idx].normal;\n"
    helpers += "        case STYLE_ITALIC: return map[idx].italic;\n"
    helpers += "        case STYLE_BOLD: return map[idx].bold;\n"
    helpers += "        case STYLE_UNDERLINE: return map[idx].underline;\n"
    helpers += "        case STYLE_STRIKETHROUGH: return map[idx].strikethrough;\n"
    helpers += "        default: return STYLE_VARIANT_ABSENT;\n"
    helpers += "    }\n"
    helpers += "}\n"
    helpers += "\n"
    helpers += "} }\n"
    with open(os.path.join(include_dir, "font_helpers.hpp"), "w", encoding=FILE_ENCODING) as f:
        f.write(helpers)
    print("[ OK ] include/font_helpers.hpp")

    # ── fonts.hpp (master aggregator) ──
    includes: typing.List[str] = []
    includes.append('#include "font_structs.hpp"')
    includes.append('#include "font_constants.hpp"')
    includes.append('#include "font_families.hpp"')
    includes.append('#include "font_helpers.hpp"')
    for name, _ in sorted(families, key=lambda x: x[0]):
        includes.append(f'#include "internal/{name}.hpp"')
    master: str = "#pragma once\n\n" + "\n".join(includes) + "\n"
    with open(os.path.join(include_dir, "fonts.hpp"), "w", encoding=FILE_ENCODING) as f:
        f.write(master)
    print("[ OK ] include/fonts.hpp")


def write_library_json(lib_dir: str) -> None:
    manifest: typing.Dict[str, typing.Any] = {
        "name": "fonts",
        "version": "1.0.0",
        "description": "Auto-generated font data for Bloated MP3 Player",
        "keywords": ["fonts", "embedded", "display"],
        "build": {
            "includeDir": "include",
            "srcDir": "src",
        },
    }
    manifest_path: str = os.path.join(lib_dir, "library.json")
    with open(manifest_path, "w", encoding=FILE_ENCODING) as handle:
        json.dump(manifest, handle, indent=2)
        handle.write("\n")
    print("[ OK ] library.json")


# ─── Per-family processing ───────────────────────────────────────────────

def process_family(
    family_name: str,
    variant_paths: typing.Dict[str, str],
    lib_dir: str,
    min_size: int,
    max_size: int,
    step: int,
    first_code: int,
    last_code: int,
    use_extern: bool,
    use_progmem: bool,
) -> typing.Tuple[int, typing.List[typing.Tuple[str, int]]]:
    if first_code < 0:
        first_code = 0
    if last_code > 0x10FFFF:
        last_code = 0x10FFFF

    # Extract cmap for each variant, compute union
    all_supported: typing.Set[int] = set()
    for _variant, fpath in variant_paths.items():
        cmap: typing.Set[int] = extract_cmap(fpath)
        all_supported |= cmap

    supported_codes: typing.List[int] = sorted(
        [c for c in all_supported if first_code <= c <= last_code]
    )
    if not supported_codes:
        print(f"  [SKIP] {family_name} — no supported code points in range")
        return 0, []

    sizes: typing.List[int] = list(range(min_size, max_size + 1, step))
    variant_names: typing.List[str] = list(variant_paths.keys())

    sizes_info: typing.List[SizeOutputInfo] = []
    last_reported: int = 0

    for pt_index, pt in enumerate(sizes):
        total_sizes: int = len(sizes)
        print(f"Rendering {family_name} {pt}pt ({pt_index + 1}/{total_sizes})...")

        variant_data: typing.Dict[str, typing.Tuple[typing.List[int], typing.List[int]]] = {}
        variants_rendered: typing.Dict[str, RenderedSize] = {}
        overall_max_w: int = 0
        overall_max_h: int = 0

        for vname in variant_names:
            fpath: str = variant_paths[vname]
            total_codes: int = len(supported_codes)

            def make_progress(idx: int, ttl: int) -> None:
                nonlocal last_reported
                use_progress: bool = ttl > 100
                if use_progress and (idx - last_reported >= 10 or idx == ttl - 1):
                    pct: float = (idx + 1) / ttl * 100.0
                    sys.stdout.write(f"\r    {vname}: {idx + 1}/{ttl} ({pct:.0f}%)")
                    sys.stdout.flush()
                    last_reported = idx

            size_data: RenderedSize = render_size(
                fpath, pt, supported_codes,
                progress_callback=make_progress if total_codes > 100 else None,
            )

            if total_codes > 100:
                sys.stdout.write("\n")
                sys.stdout.flush()

            if not size_data.glyphs:
                print(f"    [SKIP] {vname} variant — no glyphs")
                continue

            variants_rendered[vname] = size_data
            if size_data.cell_width > overall_max_w:
                overall_max_w = size_data.cell_width
            if size_data.cell_height > overall_max_h:
                overall_max_h = size_data.cell_height

            bits: typing.List[int]
            widths: typing.List[int]
            bits, widths = build_bits_array(size_data, supported_codes)
            variant_data[vname] = (bits, widths)

        if not variant_data:
            print(f"  [SKIP] {pt}pt — no rendered glyphs")
            continue

        style_map: typing.List[typing.List[int]] = build_style_map(
            variants_rendered, supported_codes
        )

        info: SizeOutputInfo = SizeOutputInfo(
            pt=pt,
            cell_width=overall_max_w,
            cell_height=overall_max_h,
            codes=supported_codes,
            variant_data=variant_data,
            style_map=style_map,
            variant_names=list(variant_data.keys()),
        )
        sizes_info.append(info)

    if not sizes_info:
        return 0, []

    size_tags: typing.List[str] = []
    for info in sizes_info:
        tag: str = _write_family_size_header_and_source(
            lib_dir, family_name, info, use_extern, use_progmem,
        )
        size_tags.append(tag)

    _write_family_aggregator(lib_dir, family_name, size_tags)

    glyph_count: int = len(supported_codes)
    return glyph_count, [(family_name, len(sizes_info))]


# ─── Font discovery ──────────────────────────────────────────────────────

def fonts_in_dir(
    input_dir: str,
) -> typing.List[str]:
    fonts: typing.List[str] = []
    for dirpath, _dirnames, filenames in os.walk(input_dir):
        for fname in filenames:
            ext: str = os.path.splitext(fname)[1].lower()
            if ext in (".ttf", ".otf"):
                fonts.append(os.path.join(dirpath, fname))
    return sorted(fonts)


def _is_variable_font_path(fpath: str) -> bool:
    lower: str = os.path.basename(fpath).lower()
    return "-variablefont" in lower


def group_families(
    font_paths: typing.List[str],
) -> typing.Dict[str, typing.Dict[str, str]]:
    families: typing.Dict[str, typing.Dict[str, str]] = {}
    variable_families: typing.Set[str] = set()

    for fpath in font_paths:
        base_name, variant = parse_variant(fpath)
        fam_key: str = sanitise_name(base_name)

        # Track whether this family has a variable font
        if _is_variable_font_path(fpath):

            # If we already have static variants for this family, replace them
            families[fam_key] = {}
            families[fam_key][variant] = fpath
            variable_families.add(fam_key)
            print(f"  [VARIABLE] {os.path.basename(fpath)} — preferred over static variants")
            continue

        # Skip static variants for families that already have a variable font
        if fam_key in variable_families:
            print(f"  [SKIP] {os.path.basename(fpath)} — family has variable font preference")
            continue

        # Variable font detection as a safety net (fvar table)
        if is_variable_font(fpath):
            print(f"  [SKIP] {os.path.basename(fpath)} — variable font (fvar table)")
            continue

        if fam_key not in families:
            families[fam_key] = {}
        if variant not in families[fam_key]:
            families[fam_key][variant] = fpath
        else:
            print(f"  [SKIP] {os.path.basename(fpath)} — duplicate variant '{variant}'")
    return families


# ─── Main ────────────────────────────────────────────────────────────────

def main() -> None:
    start_time: float = time.time()
    start_dt: str = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")

    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Convert TTF/OTF fonts to C byte arrays for embedded displays"
    )
    parser.add_argument("input", help="Input TTF/OTF file, or directory (with --batch)")
    parser.add_argument("output", help="Output directory")
    parser.add_argument(
        "--size", type=int, default=None,
        help="Single point size (overrides --min/--max)"
    )
    parser.add_argument(
        "--min", type=int, default=8, help="Minimum point size (default: 8)"
    )
    parser.add_argument(
        "--max", type=int, default=20, help="Maximum point size (default: 20)"
    )
    parser.add_argument(
        "--step", type=int, default=2,
        help="Step between successive point sizes (default: 2)"
    )
    parser.add_argument(
        "--first", type=int, default=32,
        help="First Unicode code point (default: 32 = space)"
    )
    parser.add_argument(
        "--last", type=int, default=0x10FFFF,
        help="Last Unicode code point (default: 0x10FFFF)"
    )
    parser.add_argument(
        "--batch", action="store_true",
        help="Recursively process all TTF/OTF files in input directory"
    )
    parser.add_argument(
        "--no-extern", dest="no_extern", action="store_true", default=False,
        help="Inline data in headers (no separate .cpp)"
    )
    parser.add_argument(
        "--no-progmem", dest="no_progmem", action="store_true", default=False,
        help="Omit PROGMEM attribute from arrays"
    )
    args: argparse.Namespace = parser.parse_args()

    min_size: int = args.size if args.size is not None else args.min
    max_size: int = args.size if args.size is not None else args.max
    use_extern: bool = not args.no_extern
    use_progmem: bool = not args.no_progmem

    print(f"Started at {start_dt}")
    print()

    if args.batch:
        font_paths: typing.List[str] = fonts_in_dir(args.input)
        if not font_paths:
            print("No TTF/OTF files found in", args.input, file=sys.stderr)
            sys.exit(1)
        families: typing.Dict[str, typing.Dict[str, str]] = group_families(font_paths)
        if not families:
            print("No usable font families found.", file=sys.stderr)
            sys.exit(1)

        print(f"\nFound {len(families)} font families, processing...\n")

        all_family_info: typing.List[typing.Tuple[str, int]] = []
        all_sizes: typing.List[int] = []
        total_glyphs: int = 0

        for fam_key, variant_paths in sorted(families.items()):
            print(f"=== {fam_key} ===")
            glyph_count, family_info = process_family(
                fam_key, variant_paths, args.output,
                min_size, max_size, args.step,
                args.first, args.last,
                use_extern, use_progmem,
            )
            if glyph_count > 0:
                total_glyphs += glyph_count
                all_family_info.extend(family_info)
                for pt in range(min_size, max_size + 1, args.step):
                    if pt not in all_sizes:
                        all_sizes.append(pt)

        if all_family_info:
            write_library_metadata(args.output, all_sizes, all_family_info)
            write_library_json(args.output)

        elapsed: float = time.time() - start_time
        print(f"\nDone: {len(families)} families, {total_glyphs} glyphs per size")
        print(f"Elapsed: {elapsed:.1f}s")

    else:
        base_name, _variant = parse_variant(args.input)
        family_name: str = sanitise_name(base_name)
        variants: typing.Dict[str, str] = {"normal": args.input}

        print(f"\n=== {family_name} === ")
        glyph_count, family_info = process_family(
            family_name, variants, args.output,
            min_size, max_size, args.step,
            args.first, args.last,
            use_extern, use_progmem,
        )

        if glyph_count > 0:
            all_sizes = list(range(min_size, max_size + 1, args.step))
            write_library_metadata(args.output, all_sizes, family_info)
            write_library_json(args.output)

        elapsed = time.time() - start_time
        print(f"\nDone: {glyph_count} glyphs per size written to {args.output}")
        print(f"Elapsed: {elapsed:.1f}s")


if __name__ == "__main__":
    main()
