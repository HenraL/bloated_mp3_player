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
# LAST Modified: 1:12:36 17-07-2026
# DESCRIPTION:
# This is the code in charge of making the bloated player come to life.
# Convert PNG images to C byte arrays for embedded displays.
#
# Outputs two representations per image:
# - 1-bit packed bitmap (XBM-style, for monochrome LCDs)
# - 8-bit grayscale (for future dithering / grayscale displays)
#
# Usage:
#   # single file -> stdout (static const)
#   python convert.py image.png
#
#   # batch: generate extern headers + one source file
#   python convert.py --batch input_dir/ output_dir/ --gen-source icons.cpp
#
#   # batch: generate only per-file headers (static const, original mode)
#   python convert.py --batch input_dir/ output_dir/
#
#   # batch: generate a PlatformIO library with include/ and src/
#   python convert.py --batch input_dir/ output_lib/ --library
#
# Depends: Pillow (PIL)
# /STOP
# COPYRIGHT: (c) Henry Letellier
# PURPOSE: This is the program in charge of generating the embeddable version
#          of the assets so that they can be compiled as code and don't need
#          to be provided by the sd card.
# // AR
# +==== END Bloated MP3 Player =================+
""" 

import os
import sys
import json
import argparse
from dataclasses import dataclass
from typing import List, Optional, Tuple, Dict, Any
from PIL import Image

# ─── Floyd-Steinberg error diffusion weights ──────────────────────────────
# The divisor (16) is the sum of all four coefficients (7+3+5+1) — this
# conserves total energy: every unit of quantisation error gets fully
# redistributed to the neighbouring pixels yet to be processed.
FS_DIVISOR: float = 16.0
FS_RIGHT: float = 7.0 / FS_DIVISOR
FS_BOTTOM_LEFT: float = 3.0 / FS_DIVISOR
FS_BOTTOM: float = 5.0 / FS_DIVISOR
FS_BOTTOM_RIGHT: float = 1.0 / FS_DIVISOR

# ─── Quantisation thresholds ─────────────────────────────────────────────
GRAY_THRESHOLD: int = 127
WHITE_LEVEL: int = 255

# ─── Output formatting ───────────────────────────────────────────────────
BYTES_PER_LINE: int = 16

# ─── Bit-packing group size ──────────────────────────────────────────────
BITS_PER_BYTE: int = 8

# ─── File encoding ───────────────────────────────────────────────────────
FILE_ENCODING: str = "utf-8"


def pixel_index(x: int, y: int, row_width: int) -> int:
    """Convert (x, y) grid coordinates to a 1-D index in row-major order.

    A 2-D grid of width*height is stored flat as a single list.
    This function maps (column, row) → offset so the caller never
    writes raw multiplication.
    """
    return y * row_width + x


def sanitise_name(filepath: str) -> str:
    """Derive a valid C identifier from a file path.

    Strips the extension, replaces non-alphanumeric characters with
    underscores, and prepends an underscore if the name starts with a digit.
    Falls back to 'image' if the result would be empty.
    """
    base: str = os.path.splitext(os.path.basename(filepath))[0]
    clean: List[str] = []
    for ch in base:
        if ch.isalnum() or ch == '_':
            clean.append(ch)
        else:
            clean.append('_')
    name: str = ''.join(clean)
    if name and name[0].isdigit():
        name = '_' + name
    if name:
        return name
    return 'image'


@dataclass
class IconEntry:
    """Represents one processed icon's metadata and data arrays."""
    name: str
    width: int
    height: int
    packed: Optional[List[int]]
    gray: Optional[List[int]]


class ImageConverter:
    """Converts PNG images to C byte arrays for embedded displays.

    Typical usage:
        converter = ImageConverter(output_1bit=True, output_gray=True)
        converter.batch_convert("input_pngs/", "output_headers/")
    """

    def __init__(self, output_1bit: bool = True, output_gray: bool = True) -> None:
        self.output_1bit: bool = output_1bit
        self.output_gray: bool = output_gray

    # ── Image loading ────────────────────────────────────────────────────

    def load_and_prepare(self, path: str) -> Image.Image:
        """Open a PNG and return a clean 8-bit grayscale (mode 'L') image.

        Handles RGBA, LA and PA modes by compositing over a white
        background before converting to grayscale.  Paletted (P) and
        true-grayscale images are converted directly.
        """
        img: Image.Image = Image.open(path)
        if img.mode in ("RGBA", "LA", "PA"):
            background: Image.Image = Image.new("L", img.size, WHITE_LEVEL)
            if img.mode == "LA":
                lightness, alpha = img.split()
                img = Image.composite(lightness, background, alpha)
            elif img.mode == "RGBA":
                red, green, blue, alpha = img.split()
                merged_rgb: Image.Image = Image.merge("RGB", (red, green, blue))
                gray_with_alpha: Image.Image = Image.merge(
                    "LA", (merged_rgb.convert("L"), alpha)
                )
                lightness, alpha = gray_with_alpha.split()
                img = Image.composite(lightness, background, alpha)
            else:
                img = img.convert("L")
        else:
            img = img.convert("L")
        return img

    def get_image_data(self, img: Image.Image) -> Tuple[int, int, Optional[List[int]], Optional[List[int]]]:
        """Extract raw pixel data from a prepared PIL image.

        Returns (width, height, packed_bits, gray_data) where:
        - packed_bits is the 1-bit XBM representation (or None if skipped)
        - gray_data is the raw 8-bit grayscale bytes (or None if skipped)
        """
        width: int
        height: int
        width, height = img.size
        raw: List[int] = list(img.tobytes())
        packed: Optional[List[int]] = None
        if self.output_1bit:
            dithered: List[int] = self.quantise_1bit(raw, width, height)
            packed = self.pack_bits(dithered)
        gray: Optional[List[int]] = None
        if self.output_gray:
            gray = raw
        return width, height, packed, gray

    # ── Dithering and bit-packing ────────────────────────────────────────

    @staticmethod
    def quantise_1bit(gray_flat: List[int], width: int, height: int) -> List[int]:
        """Floyd-Steinberg error diffusion dithering.

        Converts an 8-bit grayscale image (list of ints 0-255) to pure
        black-and-white (0 or 255) while distributing quantisation error to
        neighbouring pixels.  This gives the illusion of continuous tone on
        a 1-bit display.
        """
        working: List[float] = []
        for value in gray_flat:
            working.append(float(value))

        output: List[int] = [0] * (width * height)

        for row in range(height):
            for column in range(width):
                current: int = pixel_index(column, row, width)
                old: float = working[current]

                if old > GRAY_THRESHOLD:
                    new_val: int = WHITE_LEVEL
                else:
                    new_val: int = 0

                output[current] = new_val
                error: float = old - new_val

                is_not_right_edge: bool = column + 1 < width
                is_not_bottom_edge: bool = row + 1 < height
                is_not_left_edge: bool = column > 0

                if is_not_right_edge:
                    right: int = pixel_index(column + 1, row, width)
                    working[right] += error * FS_RIGHT
                if is_not_bottom_edge:
                    if is_not_left_edge:
                        bottom_left: int = pixel_index(column - 1, row + 1, width)
                        working[bottom_left] += error * FS_BOTTOM_LEFT
                    bottom: int = pixel_index(column, row + 1, width)
                    working[bottom] += error * FS_BOTTOM
                    if is_not_right_edge:
                        bottom_right: int = pixel_index(column + 1, row + 1, width)
                        working[bottom_right] += error * FS_BOTTOM_RIGHT

        return output

    @staticmethod
    def pack_bits(values_0or255: List[int]) -> List[int]:
        """Pack a list of 0/255 pixel values into XBM-style byte rows.

        Every 8 consecutive pixels become one byte, MSB-first.  A pixel
        above GRAY_THRESHOLD sets the corresponding bit to 1; anything
        below sets it to 0.
        """
        output: List[int] = []
        for group_start in range(0, len(values_0or255), BITS_PER_BYTE):
            byte: int = 0
            for bit_index in range(BITS_PER_BYTE):
                pixel_index_in_list: int = group_start + bit_index
                is_past_end: bool = pixel_index_in_list >= len(values_0or255)
                if is_past_end:
                    continue
                is_set: bool = values_0or255[pixel_index_in_list] > GRAY_THRESHOLD
                if is_set:
                    byte |= 1 << (7 - bit_index)
            output.append(byte)
        return output

    # ── C source formatting ──────────────────────────────────────────────

    @staticmethod
    def format_c_array_bytes(data: List[int], indent: str = "    ") -> List[str]:
        """Return source lines for a C uint8_t array initialiser."""
        lines: List[str] = []
        for chunk_start in range(0, len(data), BYTES_PER_LINE):
            chunk: List[int] = data[chunk_start:chunk_start + BYTES_PER_LINE]
            parts: List[str] = []
            for byte_value in chunk:
                parts.append(f"0x{byte_value:02x}")
            line: str = ", ".join(parts)
            is_last_chunk: bool = len(data) - chunk_start <= BYTES_PER_LINE
            if is_last_chunk:
                lines.append(f"{indent}{line}")
            else:
                lines.append(f"{indent}{line},")
        return lines

    # ── C header / source generation ─────────────────────────────────────

    @staticmethod
    def _build_header_text(
        name: str,
        width: int,
        height: int,
        body: str,
    ) -> str:
        """Wrap a header body in include guard, includes, and comment."""
        guard: str = name.upper()
        header_text: str = ""
        header_text += f"#ifndef ASSET_{guard}_H_\n"
        header_text += f"#define ASSET_{guard}_H_\n"
        header_text += "\n"
        header_text += "#include <stdint.h>\n"
        header_text += f"/* {name} ({width}x{height}) */\n"
        header_text += f"{body}\n"
        header_text += "\n"
        header_text += f"#endif /* ASSET_{guard}_H_ */\n"
        return header_text

    def _make_header_lines(self,name: str,width: int,height: int,packed: Optional[List[int]],gray: Optional[List[int]],use_extern: bool, use_progmem: bool = False) -> List[str]:
        """Assemble the lines of a per-icon C header.

        Generates #define macros for width/height/size and either an
        extern declaration or a static const initialiser, depending on
        use_extern.  When use_extern is True the data body is omitted
        (it will live in a shared source file). Depending on use_progmem, the arrays may be marked with PROGMEM for AVR/ESP8266 boards.
        """
        lines: List[str] = []
        lines.append(f"#define {name}_WIDTH  {width}")
        lines.append(f"#define {name}_HEIGHT {height}")
        lines.append("")

        storage:str = "static const"
        if use_extern:
            storage = "extern const" 

        if packed is not None:
            self._append_array_section(lines, storage, name, "_bits", packed, "BITS", use_extern, use_progmem)
        if gray is not None:
            self._append_array_section(lines, storage, name, "_gray", gray, "GRAY", use_extern, use_progmem)

        return lines

    @staticmethod
    def _append_array_section(lines: List[str],storage: str,name: str,suffix: str,data: List[int],size_label: str,use_extern: bool = True, use_progmem: bool = False) -> None:
        """Append one byte-array declaration (bits or gray) to *lines*."""
        progmem_str: str = " PROGMEM"
        if not use_progmem:
            progmem_str = ""
        lines.append(f"{storage} uint8_t {name}{suffix}[]{progmem_str};")
        if not use_extern:
            lines.pop()
            lines.append(f"{storage} uint8_t {name}{suffix}[] = {{")
            for line in ImageConverter.format_c_array_bytes(data):
                lines.append(line)
            lines.append(f"}}{progmem_str};")
        lines.append(f"#define {name}_{size_label}_SIZE {len(data)}")
        lines.append("")

    def generate_header(
        self,
        name: str,
        width: int,
        height: int,
        packed: Optional[List[int]],
        gray: Optional[List[int]],
        use_extern: bool = False,
        use_progmem: bool = False
    ) -> str:
        """Build the full text of a per-icon C header file."""
        return "\n".join(self._make_header_lines(name, width, height, packed, gray, use_extern, use_progmem))

    def generate_source_entry(
        self, name: str, width: int, height: int,
        packed: Optional[List[int]], gray: Optional[List[int]],
        use_progmem: bool = False,
    ) -> str:
        """Build the C source block for one icon in the shared .cpp file.

        Contains the full const uint8_t array initialisers for both the
        packed bitmap and the grayscale data, with a comment header.
        """
        progmem_attr: str = " PROGMEM" if use_progmem else ""
        lines: List[str] = []
        lines.append(f"/* {name} ({width}x{height}) */")
        if packed is not None:
            lines.append(f"const uint8_t {name}_bits[]{progmem_attr} = {{")
            for line in ImageConverter.format_c_array_bytes(packed):
                lines.append(line)
            lines.append("};")
            lines.append("")
        if gray is not None:
            lines.append(f"const uint8_t {name}_gray[]{progmem_attr} = {{")
            for line in ImageConverter.format_c_array_bytes(gray):
                lines.append(line)
            lines.append("};")
            lines.append("")
        return "\n".join(lines)

    # ── Batch processing ─────────────────────────────────────────────────

    def _process_single_png(
        self,
        png_path: str,
        output_dir: str,
        use_extern: bool = True,
        use_progmem: bool = False,
    ) -> Optional[IconEntry]:
        """Process one PNG file and write its header.

        Returns an IconEntry on success, or None on failure.
        """
        name: str = sanitise_name(png_path)
        try:
            img: Image.Image = self.load_and_prepare(png_path)
            width: int
            height: int
            packed: Optional[List[int]]
            gray: Optional[List[int]]
            width, height, packed, gray = self.get_image_data(img)

            header_path: str = os.path.join(output_dir, name + ".hpp")
            body: str = self.generate_header(name, width, height, packed, gray, use_extern, use_progmem)
            header_text: str = self._build_header_text(name, width, height, body)
            with open(header_path, "w", encoding=FILE_ENCODING) as handle:
                handle.write(header_text)

            print(f"[ OK ] {os.path.basename(png_path)} -> {os.path.basename(header_path)}")
            return IconEntry(name, width, height, packed, gray)
        except Exception as error:
            print(f"[FAIL] {os.path.basename(png_path)}: {error}", file=sys.stderr)
            return None

    def _gather_pngs(self, input_dir: str) -> List[str]:
        """Recursively collect all PNG paths under *input_dir*."""
        pngs: List[str] = []
        for dirpath, _dirnames, filenames in os.walk(input_dir):
            for fname in filenames:
                if fname.lower().endswith(".png"):
                    pngs.append(os.path.join(dirpath, fname))
        return sorted(pngs)

    def _write_shared_source(
        self, entries: List[IconEntry], source_path: str,
        include_headers: Optional[List[str]] = None,
        use_progmem: bool = False,
    ) -> None:
        """Write a single .cpp containing all icon data arrays."""
        source_lines: List[str] = [
            '#include <Arduino.h>',
        ]
        if include_headers:
            for h in include_headers:
                source_lines.append(f'#include "{h}"')
        source_lines.append('')
        for entry in entries:
            block: str = self.generate_source_entry(
                entry.name, entry.width, entry.height,
                entry.packed, entry.gray,
                use_progmem=use_progmem,
            )
            source_lines.append(block)
        with open(source_path, "w", encoding=FILE_ENCODING) as handle:
            handle.write("\n".join(source_lines) + "\n")
        print(f"[ OK ] source -> {source_path} ({len(entries)} icons)")

    def batch_convert(
        self,
        input_dir: str,
        output_dir: str,
        gen_source: Optional[str] = None,
        use_extern: bool = True,
        use_progmem: bool = False,
    ) -> int:
        """Process every PNG under *input_dir* recursively.

        Headers are written to *output_dir*.  When *gen_source* is set,
        a single .cpp containing all array data is also generated.

        Returns the number of successfully converted icons.
        """
        os.makedirs(output_dir, exist_ok=True)
        entries: List[IconEntry] = []

        pngs: List[str] = self._gather_pngs(input_dir)
        for png_path in pngs:
            entry: Optional[IconEntry] = self._process_single_png(
                png_path, output_dir, use_extern=use_extern, use_progmem=use_progmem,
            )
            if entry is not None:
                entries.append(entry)

        if use_extern and gen_source and entries:
            if os.path.isabs(gen_source):
                source_path: str = gen_source
                include_prefix: str = os.path.relpath(output_dir, os.path.dirname(source_path))
            else:
                source_dir: str = os.path.normpath(os.path.join(output_dir, "..", "src"))
                os.makedirs(source_dir, exist_ok=True)
                source_path = os.path.join(source_dir, gen_source)
                include_prefix = os.path.relpath(output_dir, source_dir)
            include_headers: List[str] = []
            for entry in entries:
                include_headers.append(os.path.join(include_prefix, entry.name + ".hpp"))
            self._write_shared_source(
                entries, source_path, include_headers=include_headers,
                use_progmem=use_progmem,
            )

        return len(entries)

    # ── Library mode (PlatformIO library output) ─────────────────────────

    def _write_master_header(
        self, entries: List[IconEntry], include_dir: str
    ) -> None:
        """Generate a top-level images.hpp that includes every per-icon header."""
        includes: List[str] = []
        for entry in entries:
            includes.append(f'#include "internal/{entry.name}.hpp"')
        includes.sort()
        content: str = ""
        content+="#pragma once\n"
        content+="\n"
        content+="/* Auto-generated master header — includes every icon. */\n"
        content+="\n"
        content+=f"{'\n'.join(includes)}"
        content+="\n"
        master_path: str = os.path.join(include_dir, "images.hpp")
        with open(master_path, "w", encoding=FILE_ENCODING) as handle:
            handle.write(content)

    def _write_library_json(self, lib_dir: str) -> None:
        """Write a platformio-compatible library.json."""
        manifest: Dict[str, Any] = {
            "name": "images",
            "version": "1.0.0",
            "description": "Auto-generated image and font data for Bloated MP3 Player",
            "keywords": ["images", "embedded", "bloated-mp3"],
            "build": {
                "includeDir": "include",
                "srcDir": "src",
            },
        }
        manifest_path: str = os.path.join(lib_dir, "library.json")
        with open(manifest_path, "w", encoding=FILE_ENCODING) as handle:
            json.dump(manifest, handle, indent=2)
            handle.write("\n")

    def library_convert(
        self,
        input_dir: str,
        lib_dir: str,
        gen_source: str = "images.cpp",
        use_extern: bool = True,
        use_progmem: bool = False,
    ) -> int:
        """Convert PNGs into a PlatformIO library under *lib_dir*.

        Structure:
            <lib_dir>/
            ├── library.json
            ├── include/
            │   ├── images.hpp        (master header)
            │   └── internal/
            │       ├── icon1.hpp
            │       └── ...
            └── src/
                └── images.cpp         (data definitions)

        Returns the number of successfully converted icons.
        """
        include_dir: str = os.path.join(lib_dir, "include")
        internal_dir: str = os.path.join(include_dir, "internal")
        source_dir: str = os.path.join(lib_dir, "src")

        os.makedirs(internal_dir, exist_ok=True)
        os.makedirs(source_dir, exist_ok=True)

        entries: List[IconEntry] = []
        pngs: List[str] = self._gather_pngs(input_dir)

        for png_path in pngs:
            name: str = sanitise_name(png_path)
            try:
                img: Image.Image = self.load_and_prepare(png_path)
                width: int
                height: int
                packed: Optional[List[int]]
                gray: Optional[List[int]]
                width, height, packed, gray = self.get_image_data(img)

                header_path: str = os.path.join(internal_dir, name + ".hpp")
                body: str = self.generate_header(name, width, height, packed, gray, use_extern, use_progmem)
                header_text: str = self._build_header_text(name, width, height, body)
                with open(header_path, "w", encoding=FILE_ENCODING) as handle:
                    handle.write(header_text)

                print(f"[ OK ] {os.path.basename(png_path)} -> internal/{name}.hpp")
                entries.append(IconEntry(name, width, height, packed, gray))
            except Exception as error:
                print(f"[FAIL] {os.path.basename(png_path)}: {error}", file=sys.stderr)

        if entries:
            self._write_master_header(entries, include_dir)
            if use_extern:
                source_path: str = os.path.join(source_dir, gen_source)
                self._write_shared_source(
                    entries, source_path,
                    include_headers=["images.hpp"],
                    use_progmem=use_progmem,
                )
            self._write_library_json(lib_dir)

        return len(entries)

    # ── Single-file conversion ───────────────────────────────────────────

    def convert_single(
        self, input_path: str, output_path: Optional[str] = None,
        use_progmem: bool = False,
    ) -> None:
        """Convert one PNG file and print (or write) its C header."""
        img: Image.Image = self.load_and_prepare(input_path)
        name: str = sanitise_name(input_path)
        width: int
        height: int
        packed: Optional[List[int]]
        gray: Optional[List[int]]
        width, height, packed, gray = self.get_image_data(img)
        body: str = self.generate_header(name, width, height, packed, gray, use_extern=False, use_progmem=use_progmem)
        if output_path:
            header_text: str = self._build_header_text(name, width, height, body)
            with open(output_path, "w", encoding=FILE_ENCODING) as handle:
                handle.write(header_text)
            print(f"Wrote {output_path}")
        else:
            print(body)


def main() -> None:
    """Entry point: parse arguments and drive conversion."""
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Convert PNG images to C byte arrays for embedded displays"
    )
    parser.add_argument("input", help="Input PNG file or directory (with --batch)")
    parser.add_argument("-o", "--output", help="Output file (single) or directory (batch)")
    parser.add_argument(
        "--batch", action="store_true",
        help="Batch convert all PNGs in directory (recursive)"
    )
    parser.add_argument(
        "--library", action="store_true",
        help="Generate a PlatformIO library structure with include/ + src/"
    )
    parser.add_argument("--no-gray", action="store_true", help="Skip 8-bit grayscale array")
    parser.add_argument("--no-bits", action="store_true", help="Skip 1-bit packed array")
    parser.add_argument(
        "--gen-source", metavar="FILE",
        help="Generate a single .cpp with all definitions (implies extern in headers)"
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

    if args.no_gray and args.no_bits:
        print("Error: both --no-gray and --no-bits specified, nothing to output")
        sys.exit(1)

    use_extern: bool = not args.no_extern
    use_progmem: bool = not args.no_progmem

    converter: ImageConverter = ImageConverter(
        output_1bit=not args.no_bits,
        output_gray=not args.no_gray,
    )

    if args.library:
        lib_dir: str = args.output or os.path.join(args.input, "..", "fonts")
        count: int = converter.library_convert(
            args.input, lib_dir, gen_source="images.cpp",
            use_extern=use_extern, use_progmem=use_progmem,
        )
        print(f"Converted {count} icons to library at {lib_dir}")
    elif args.batch:
        output_dir: str = args.output or os.path.join(args.input, "..", "generated")
        count = converter.batch_convert(
            args.input, output_dir, gen_source=args.gen_source,
            use_extern=use_extern, use_progmem=use_progmem,
        )
        print(f"Converted {count} icons to {output_dir}")
    else:
        converter.convert_single(args.input, args.output, use_progmem=use_progmem)


if __name__ == "__main__":
    main()
