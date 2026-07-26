"""
Flamegraph renderer for Bloated MP3 Player profiling data.

Generates interactive SVG flamegraphs with dark theme, flame coloring,
hover tooltips, and click-to-zoom.

Usage:
  python flamegraph.py < serial_dump.txt         Read from stdin
  python flamegraph.py serial_dump.txt           Process single file
  python flamegraph.py --scan <directory>        Scan directory for profiling macros
  python flamegraph.py --scan <dir> -o out.svg   Scan + custom output basename
  python flamegraph.py -h                        Show this help

Modes:
  Stdin/file mode:  Parses serial dump text with PROFILING sections.
  --scan mode:      Parses .cpp/.hpp source files for PROFILE_BLOCK macros
                    and generates a flamegraph showing profiling structure.

Input format (serial dump):
  === PROFILING TRACES ===
  my_func 12345
  === END PROFILING TRACES ===

Output: Interactive SVG flamegraph(s) with dark theme, flame colours,
hover tooltips, and click-to-zoom.
"""

import sys
import os
import re
import html
from collections import defaultdict

WIDTH = 1200
BAR_HEIGHT = 17
PADDING = 2
HEADER_HEIGHT = 80
FOOTER_HEIGHT = 30
MIN_BAR_WIDTH = 1
FONT_FAMILY = "'SF Mono','Monaco','Inconsolata','Fira Mono','Droid Sans Mono','Source Code Pro',monospace"


def print_help():
    h = """flamegraph.py — Interactive SVG flamegraph generator

Usage:
  python flamegraph.py < serial_dump.txt         Read from stdin
  python flamegraph.py serial_dump.txt           Process single file
  python flamegraph.py --scan <directory>        Scan source code for profiling
  python flamegraph.py --scan <dir> -o out.svg   Scan + custom output basename
  python flamegraph.py -h                        Show this help

Options:
  -h, --help            Show this help message
  --scan <directory>    Scan .cpp/.hpp files for PROFILE_BLOCK macros and
                        generate flamegraph from source code structure
  -o <filename>         Output SVG filename (default: flamegraph.svg)

Scan mode (--scan):
  Parses C++ source files looking for PROFILE_BLOCK(name),
  PROFILE_BEGIN(name), and PROFILE_END(name) macros. Builds a
  structural flamegraph showing where profiling instrumentation
  lives in the codebase.

  Each frame shows:  file:function → profiling_block_name

Input format (stdin/file mode):
  === PROFILING TRACES ===
  label_name  duration_us
  another_func 12345
  === END PROFILING TRACES ===

  === PROFILING TASK STATS ===
  TaskName  cpu_ticks
  === END PROFILING TASK STATS ===

Examples:
  python flamegraph.py < serial_dump.txt
  python flamegraph.py serial_dump.txt -o cpu.svg
  python flamegraph.py --scan ../src/
  python flamegraph.py --scan ../ --output profile.svg
"""
    print(h)


# ─── Serial-dump parsing ───────────────────────────────────────────────

def parse_sections(text):
    sections = {}
    pattern = r"=== (PROFILING [\w ]+) ===\n(.*?)\n=== END \1 ==="
    for m in re.finditer(pattern, text, re.DOTALL):
        name = m.group(1)
        body = m.group(2).strip()
        entries = []
        for line in body.splitlines():
            parts = line.rsplit(None, 1)
            if len(parts) == 2:
                try:
                    val = int(parts[1])
                    entries.append((parts[0], val))
                except ValueError:
                    pass
        sections[name] = entries
    return sections


# ─── C++ source code parsing ───────────────────────────────────────────

def is_comment_line(line, in_block_comment):
    stripped = line.strip()
    if in_block_comment:
        if "*/" in stripped:
            return False, False
        return True, True
    if stripped.startswith("//"):
        return True, False
    if "/*" in stripped:
        if "*/" not in stripped:
            return True, True
    return False, False


def strip_comments_and_strings(text):
    result = []
    i = 0
    in_block = False
    in_line = False
    in_string = False
    string_char = None
    while i < len(text):
        c = text[i]
        if in_string:
            result.append(c)
            if c == '\\':
                result.append(text[i+1])
                i += 2
                continue
            if c == string_char:
                in_string = False
            i += 1
            continue
        if c in ('"', "'"):
            in_string = True
            string_char = c
            result.append(c)
            i += 1
            continue
        if not in_block:
            if c == '/' and i + 1 < len(text) and text[i+1] == '/':
                in_line = True
                i += 2
                continue
            if c == '/' and i + 1 < len(text) and text[i+1] == '*':
                in_block = True
                i += 2
                continue
        if in_line:
            if c == '\n':
                in_line = False
                result.append(c)
            i += 1
            continue
        if in_block:
            if c == '*' and i + 1 < len(text) and text[i+1] == '/':
                in_block = False
                i += 2
                continue
            i += 1
            continue
        result.append(c)
        i += 1
    return ''.join(result)


PROFILE_BLOCK_RE = re.compile(r'PROFILE_BLOCK\s*\(\s*"([^"]*)"\s*\)')
PROFILE_BEGIN_RE = re.compile(r'PROFILE_BEGIN\s*\(\s*"([^"]*)"\s*\)')
PROFILE_END_RE = re.compile(r'PROFILE_END\s*\(\s*"([^"]*)"\s*\)')

FUNC_SIG_RE = re.compile(
    r'(?:[\w:~\*&<>,\s]+?\s+)?'      # optional return type + qualifiers
    r'(\w+)'                          # function name (capture)
    r'\s*\([^)]*\)'                   # parameters
    r'\s*(?:const\s*)?'               # optional const
    r'\s*(?:override\s*)?'            # optional override
    r'\s*$'                           # end of line
)

KEYWORD_SET = frozenset({
    'if', 'while', 'for', 'switch', 'catch', 'else',
    'return', 'delete', 'sizeof', 'throw', 'try',
})


def find_functions(clean_text):
    """Walk cleaned source line-by-line, track brace depth, find functions.

    Handles Allman style (brace on next line) and K&R style (brace on
    same line).

    Returns list of (name, start_line) where start_line is the line
    (0-indexed) in the cleaned text where the function definition starts.
    """
    lines = clean_text.splitlines()
    functions = []
    prev_sig = None
    prev_sig_li = -1

    for li, line in enumerate(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith('#'):
            continue

        if '{' in stripped:
            if prev_sig is not None:
                functions.append((prev_sig, prev_sig_li))
                prev_sig = None
                prev_sig_li = -1
                continue

            bare = stripped
            if bare.endswith('{'):
                bare = bare[:-1].strip()
            elif '{' in bare:
                parts = bare.split('{', 1)
                bare = parts[0].strip()

            m = FUNC_SIG_RE.match(bare)
            if m:
                name = m.group(1)
                if name not in KEYWORD_SET and not name.startswith('__'):
                    functions.append((name, li))

            prev_sig = None
            prev_sig_li = -1
            continue

        m = FUNC_SIG_RE.match(stripped)
        if m:
            name = m.group(1)
            if name not in KEYWORD_SET and not name.startswith('__'):
                prev_sig = name
                prev_sig_li = li
        else:
            prev_sig = None
            prev_sig_li = -1

    return functions


def find_enclosing_function(line_no, functions):
    """Find the function that contains a given line number (0-indexed)."""
    best = None
    best_start = -1
    for fname, func_line in functions:
        if func_line <= line_no and func_line > best_start:
            best = fname
            best_start = func_line
    return best or '(global)'


def parse_source_file(filepath):
    """Parse a .cpp or .hpp file for profiling macros.

    Returns dict:
      file: filepath (relative)
      blocks: list of (name, line_no, enclosing_function)
      begins: list of (name, line_no, enclosing_function)
      ends: list of (name, line_no, enclosing_function)
    """
    with open(filepath, 'r') as f:
        text = f.read()

    clean = strip_comments_and_strings(text)

    blocks = []
    begins = []
    ends = []

    functions = find_functions(clean)

    clean_lines = clean.splitlines()

    for li, line in enumerate(clean_lines):
        for m in PROFILE_BLOCK_RE.finditer(line):
            name = m.group(1)
            enclosing = find_enclosing_function(li, functions)
            blocks.append((name, li, enclosing))

        for m in PROFILE_BEGIN_RE.finditer(line):
            name = m.group(1)
            enclosing = find_enclosing_function(li, functions)
            begins.append((name, li, enclosing))

        for m in PROFILE_END_RE.finditer(line):
            name = m.group(1)
            enclosing = find_enclosing_function(li, functions)
            ends.append((name, li, enclosing))

    relpath = os.path.relpath(filepath)
    return {
        'file': relpath,
        'blocks': blocks,
        'begins': begins,
        'ends': ends,
        'function_count': len(functions),
        'functions': functions,
    }


def scan_source_directory(directory):
    """Scan directory for .cpp/.hpp files and parse profiling macros.

    Returns list of parsed file dicts.
    """
    results = []
    for root, dirs, files in os.walk(directory):
        # Skip hidden dirs
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        for f in files:
            if f.endswith('.cpp') or f.endswith('.hpp'):
                fp = os.path.join(root, f)
                try:
                    parsed = parse_source_file(fp)
                    results.append(parsed)
                except Exception as e:
                    print(f"  ! {fp}: {e}", file=sys.stderr)
    return results


# ─── SVG generation ────────────────────────────────────────────────────

def flame_gradient(t):
    """Return a pair of colours (base, highlight) for a flame-like gradient."""
    if t < 0.25:
        r1, g1, b1 = 80, 20, 20
        r2, g2, b2 = 140, 40, 30
    elif t < 0.50:
        r1, g1, b1 = 160, 50, 20
        r2, g2, b2 = 220, 100, 30
    elif t < 0.75:
        r1, g1, b1 = 220, 100, 20
        r2, g2, b2 = 250, 180, 50
    else:
        r1, g1, b1 = 250, 180, 40
        r2, g2, b2 = 255, 240, 120
    return (f"#{r1:02x}{g1:02x}{b1:02x}", f"#{r2:02x}{g2:02x}{b2:02x}")


FILE_PALETTE = [
    "#e74c3c", "#3498db", "#2ecc71", "#f39c12", "#9b59b6",
    "#1abc9c", "#e67e22", "#2980b9", "#27ae60", "#d35400",
    "#8e44ad", "#16a085", "#c0392b", "#2c3e50", "#f1c40f",
]


def flamegraph_svg(entries, title, width=WIDTH):
    """Generate flamegraph from (name, value) entries (serial-dump mode)."""
    if not entries:
        return _empty_svg(title, "no data")
    total = sum(v for _, v in entries)
    if total == 0:
        return _empty_svg(title, "all zero")

    svg_w = width
    content_h = max(len(entries) * (BAR_HEIGHT + PADDING), 60)
    svg_h = HEADER_HEIGHT + content_h + FOOTER_HEIGHT

    svg_id = _safe_id(title)
    grad_id = f"grad_{svg_id}"
    lines = []
    _w = lines.append

    _w(f'<svg xmlns="http://www.w3.org/2000/svg" '
       f'width="{svg_w}" height="{svg_h}" '
       f'viewBox="0 0 {svg_w} {svg_h}"'
       f' style="background:#1a1a2e"'
       f' font-family="{FONT_FAMILY}">')

    _w('<defs>')
    _w(f'<linearGradient id="{grad_id}" x1="0" y1="0" x2="0" y2="1">'
       f'<stop offset="0%" stop-color="#2d2d5e"/>'
       f'<stop offset="100%" stop-color="#1a1a2e"/>'
       f'</linearGradient>')
    _w('<filter id="glow"><feGaussianBlur stdDeviation="1.5" result="blur"/>'
       '<feMerge><feMergeNode in="blur"/><feMergeNode in="SourceGraphic"/>'
       '</feMerge></filter>')
    _w('<filter id="shadow"><feDropShadow dx="0" dy="1" stdDeviation="1" '
       'flood-opacity="0.4"/></filter>')
    _w('</defs>')

    _w(f'<rect width="{svg_w}" height="{svg_h}" fill="url(#{grad_id})"/>')
    _w(f'<rect x="0" y="0" width="{svg_w}" height="{HEADER_HEIGHT}" '
       f'fill="#16213e" opacity="0.6"/>')
    _w(f'<line x1="0" y1="{HEADER_HEIGHT}" x2="{svg_w}" y2="{HEADER_HEIGHT}" '
       f'stroke="#0f3460" stroke-width="1"/>')

    _w(f'<text x="20" y="26" font-size="15" font-weight="700" fill="#e94560">'
       f'{html.escape(title)}</text>')
    _w(f'<text x="20" y="48" font-size="11" fill="#8899aa">'
       f'total samples: {total:,}  |  {len(entries)} entries</text>')
    _w(f'<text x="20" y="66" font-size="10" fill="#556677">'
       f'[hover for details]  [click to zoom]  '
       f'<tspan fill="#e94560">Ctrl+F</tspan> to search</text>')

    x_off = 10
    y_off = HEADER_HEIGHT + 4
    usable_w = svg_w - 20
    prev_name = None

    entries_sorted = sorted(entries, key=lambda e: e[1], reverse=True)

    for rank, (name, val) in enumerate(entries_sorted):
        bw = max(val * usable_w / total, MIN_BAR_WIDTH)

        if name != prev_name:
            prev_name = name

        pct = val * 100.0 / total
        t = pct / 100.0
        base_c, hi_c = flame_gradient(t)

        bar_id = f"b_{svg_id}_{rank}"
        tip_lines = [
            html.escape(name),
            f"Duration: {val:,} us",
            f"Share: {pct:.1f}%",
            f"Rank: #{rank + 1}",
        ]

        _w(f'<g filter="url(#shadow)">')
        _w(f'<rect id="{bar_id}" class="frame"'
           f' x="{x_off:.1f}" y="{y_off}" width="{bw:.1f}"'
           f' height="{BAR_HEIGHT}" fill="{base_c}" rx="2.5" ry="2.5"'
           f' stroke="rgba(255,255,255,0.08)" stroke-width="0.5"'
           f' onclick="zoom_{svg_id}({rank})"'
           f' onmouseover="highlight(this)"'
           f' onmouseout="unhighlight(this)">'
           f'<title>{chr(10).join(tip_lines)}</title>'
           f'</rect>')
        _w(f'<rect x="{x_off:.1f}" y="{y_off}" width="{bw:.1f}"'
           f' height="{BAR_HEIGHT}" fill="{hi_c}"'
           f' rx="2.5" ry="2.5" opacity="0.25"'
           f' style="pointer-events:none"/>'
           f'</g>')

        label_w = bw
        approx_w = (len(name) + 7) * 6.5

        if label_w >= approx_w:
            fc = "#1a1a2e" if t > 0.5 else "#ffffff"
            _w(f'<text x="{x_off + 4:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
               f' font-size="10" font-weight="500" fill="{fc}"'
               f' style="pointer-events:none">'
               f'{html.escape(name)} ({pct:.0f}%)</text>')
        elif label_w >= 45:
            _w(f'<text x="{x_off + 3:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
               f' font-size="9" fill="#fff" opacity="0.7"'
               f' style="pointer-events:none">{pct:.0f}%</text>')

        x_off += bw
        if x_off - 10 >= usable_w:
            remaining = len(entries_sorted) - rank - 1
            if remaining > 0:
                _w(f'<text x="{svg_w - 200:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
                   f' font-size="10" fill="#667" font-style="italic">'
                   f'\u2026 and {remaining} more</text>')
            break
        y_off += BAR_HEIGHT + PADDING

    _w(_zoom_script(svg_id, svg_w, svg_h))
    _w(f'<text x="20" y="{svg_h - 10}" font-size="9" fill="#445566">'
       f'generated by bloated_mp3_player flamegraph.py  |  '
       f'click bar = zoom to frame  |  click again = reset</text>')
    _w('</svg>')
    return "\n".join(lines)


def source_flamegraph_svg(parsed_files, title, width=WIDTH):
    """Generate flamegraph from parsed source code (--scan mode).

    Each profiling block becomes a frame. Frames are grouped by file,
    then by function. Width is equal for all blocks since we have no
    runtime data.
    """
    all_blocks = []
    for pf in parsed_files:
        for name, line, func in pf['blocks']:
            all_blocks.append((pf['file'], func, name, line))
        for name, line, func in pf['begins']:
            all_blocks.append((pf['file'], func, f"BEGIN:{name}", line))

    if not all_blocks:
        return _empty_svg(title, "no profiling macros found")

    svg_w = width
    block_count = len(all_blocks)
    content_h = max(block_count * (BAR_HEIGHT + PADDING), 60)
    svg_h = HEADER_HEIGHT + content_h + FOOTER_HEIGHT

    svg_id = _safe_id(title)
    grad_id = f"grad_{svg_id}"

    file_colors = {}
    fc_idx = 0

    lines = []
    _w = lines.append

    _w(f'<svg xmlns="http://www.w3.org/2000/svg" '
       f'width="{svg_w}" height="{svg_h}" '
       f'viewBox="0 0 {svg_w} {svg_h}"'
       f' style="background:#1a1a2e"'
       f' font-family="{FONT_FAMILY}">')

    _w('<defs>')
    _w(f'<linearGradient id="{grad_id}" x1="0" y1="0" x2="0" y2="1">'
       f'<stop offset="0%" stop-color="#2d2d5e"/>'
       f'<stop offset="100%" stop-color="#1a1a2e"/>'
       f'</linearGradient>')
    _w('<filter id="shadow"><feDropShadow dx="0" dy="1" stdDeviation="1" '
       'flood-opacity="0.4"/></filter>')
    _w('</defs>')

    _w(f'<rect width="{svg_w}" height="{svg_h}" fill="url(#{grad_id})"/>')
    _w(f'<rect x="0" y="0" width="{svg_w}" height="{HEADER_HEIGHT}" '
       f'fill="#16213e" opacity="0.6"/>')
    _w(f'<line x1="0" y1="{HEADER_HEIGHT}" x2="{svg_w}" y2="{HEADER_HEIGHT}" '
       f'stroke="#0f3460" stroke-width="1"/>')

    file_count = len(set(pf['file'] for pf in parsed_files))
    func_count = sum(pf['function_count'] for pf in parsed_files)
    _w(f'<text x="20" y="26" font-size="15" font-weight="700" fill="#e94560">'
       f'{html.escape(title)}</text>')
    _w(f'<text x="20" y="48" font-size="11" fill="#8899aa">'
       f'{block_count} profiling blocks  |  {file_count} files  |  '
       f'{func_count} functions</text>')
    _w(f'<text x="20" y="66" font-size="10" fill="#556677">'
       f'[hover for details]  [click to zoom]</text>')

    usable_w = svg_w - 20
    x_start = 10

    total_weight = block_count
    y_off = HEADER_HEIGHT + 4

    _w(f'<rect x="0" y="{HEADER_HEIGHT}" width="{svg_w}" height="'
       f'{content_h}" fill="none"/>')

    blocks_sorted = sorted(all_blocks, key=lambda b: (b[0], b[1], b[3]))

    prev_file = None
    for idx, (filepath, func, bname, line) in enumerate(blocks_sorted):
        if filepath not in file_colors:
            file_colors[filepath] = FILE_PALETTE[fc_idx % len(FILE_PALETTE)]
            fc_idx += 1

        color = file_colors[filepath]

        if filepath != prev_file:
            prev_file = filepath
            _w(f'<text x="{x_start}" y="{y_off - 3}" font-size="8" '
               f'fill="#445566" font-style="italic">{html.escape(filepath)}</text>')

        bw = (usable_w - 20) / block_count
        bw = max(bw, MIN_BAR_WIDTH)

        bar_id = f"src_b_{svg_id}_{idx}"
        display_name = f"{func} \u2192 {bname}" if func != '(global)' else bname
        tip_lines = [
            f"File: {filepath}",
            f"Function: {func}",
            f"Block: {bname}",
            f"Line: {line + 1}",
        ]

        _w(f'<g filter="url(#shadow)">')
        _w(f'<rect id="{bar_id}" class="frame"'
           f' x="{x_start:.1f}" y="{y_off}" width="{bw:.1f}"'
           f' height="{BAR_HEIGHT}" fill="{color}" rx="2.5" ry="2.5"'
           f' stroke="rgba(255,255,255,0.1)" stroke-width="0.5"'
           f' onclick="zoom_{svg_id}({idx})"'
           f' onmouseover="highlight(this)"'
           f' onmouseout="unhighlight(this)">'
           f'<title>{chr(10).join(tip_lines)}</title>'
           f'</rect>'
           f'</g>')

        label = f"{func}:{bname}"
        approx_w = len(label) * 6 + 4
        if bw >= approx_w:
            _w(f'<text x="{x_start + 3:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
               f' font-size="9" fill="#fff"'
               f' style="pointer-events:none">{html.escape(label)}</text>')

        x_start += bw
        y_off += BAR_HEIGHT + PADDING

    _w(_zoom_script(svg_id, svg_w, svg_h))
    _w(f'<text x="20" y="{svg_h - 10}" font-size="9" fill="#445566">'
       f'generated by bloated_mp3_player flamegraph.py  |  '
       f'click bar = zoom  |  click again = reset</text>')
    _w('</svg>')
    return "\n".join(lines)


def _zoom_script(svg_id, svg_w, svg_h):
    return f'''<script type="text/ecmascript"><![CDATA[
var svg_{svg_id} = document.querySelector("svg");
var ow_{svg_id} = {svg_w};
var oh_{svg_id} = {svg_h};
var z_{svg_id} = false;
function zoom_{svg_id}(idx) {{
    var el = document.getElementById("src_b_{svg_id}_"+idx) ||
             document.getElementById("b_{svg_id}_"+idx);
    if (!el) return;
    if (z_{svg_id}) {{
        svg_{svg_id}.setAttribute("viewBox","0 0 "+ow_{svg_id}+" "+oh_{svg_id});
        z_{svg_id} = false; return;
    }}
    var x = parseFloat(el.getAttribute("x"));
    var w = parseFloat(el.getAttribute("width"));
    var y = parseFloat(el.getAttribute("y"));
    svg_{svg_id}.setAttribute("viewBox",
        Math.max(0,x-10)+" "+Math.max(0,y-10)+" "+
        Math.min(ow_{svg_id},w+20)+" "+oh_{svg_id});
    z_{svg_id} = true;
}}
function highlight(el) {{
    el.setAttribute("stroke","#ffffff");
    el.setAttribute("stroke-width","1.5");
}}
function unhighlight(el) {{
    el.setAttribute("stroke","rgba(255,255,255,0.08)");
    el.setAttribute("stroke-width","0.5");
}}
]]></script>'''


def _safe_id(s):
    return re.sub(r'[^a-zA-Z0-9_]', '_', s)


def _empty_svg(title, msg):
    return (f'<svg xmlns="http://www.w3.org/2000/svg" '
            f'width="{WIDTH}" height="100"'
            f' style="background:#1a1a2e"'
            f' font-family="{FONT_FAMILY}">'
            f'<rect width="{WIDTH}" height="100" fill="#1a1a2e"/>'
            f'<text x="20" y="40" font-size="14" fill="#e94560" '
            f'font-weight="bold">{html.escape(title)}</text>'
            f'<text x="20" y="65" font-size="12" fill="#667">'
            f'{html.escape(msg)}</text></svg>')


# ─── I/O ───────────────────────────────────────────────────────────────

def process_serial_dump(infile, outfile):
    if infile:
        with open(infile) as f:
            text = f.read()
    else:
        text = sys.stdin.read()
    sections = parse_sections(text)
    if not sections:
        print("No profiling sections found in input.", file=sys.stderr)
        sys.exit(1)
    for section_name, entries in sections.items():
        tag = section_name.replace("PROFILING ", "").lower().replace(" ", "_")
        out_name = outfile
        if len(sections) > 1:
            base, ext = os.path.splitext(outfile)
            out_name = f"{base}_{tag}{ext}"
        svg = flamegraph_svg(entries, section_name)
        with open(out_name, "w") as f:
            f.write(svg)
        print(f"Wrote {out_name} ({len(entries)} entries)")


def process_scan(directory, outfile_base):
    if not os.path.isdir(directory):
        print(f"Error: '{directory}' is not a directory.", file=sys.stderr)
        sys.exit(1)

    print(f"Scanning source files in '{directory}' ...")
    parsed = scan_source_directory(directory)

    if not parsed:
        print("No .cpp or .hpp files found.", file=sys.stderr)
        sys.exit(1)

    total_blocks = sum(len(p['blocks']) + len(p['begins']) for p in parsed)
    total_files = len(parsed)

    print(f"  {total_files} files, {total_blocks} profiling macros found\n")

    tag = "source_profile"
    out_name = f"{outfile_base}_{tag}.svg" if outfile_base != "flamegraph" else f"flamegraph_{tag}.svg"
    svg = source_flamegraph_svg(parsed, f"Source Profile — {directory}")
    with open(out_name, "w") as f:
        f.write(svg)
    print(f"Wrote {out_name} ({total_blocks} blocks, {total_files} files)")


def main():
    args = sys.argv[1:]

    if not args or "-h" in args or "--help" in args:
        print_help()
        sys.exit(0)

    if "--scan" in args:
        idx = args.index("--scan")
        if idx + 1 >= len(args):
            print("Error: --scan requires a directory argument.", file=sys.stderr)
            sys.exit(1)
        directory = args[idx + 1]
        outfile_base = "flamegraph"
        if "-o" in args:
            oi = args.index("-o")
            if oi + 1 < len(args):
                outfile_base = args[oi + 1]
        process_scan(directory, outfile_base)
        return

    infile = None
    outfile = "flamegraph.svg"

    i = 0
    while i < len(args):
        a = args[i]
        if a == "-o":
            if i + 1 < len(args):
                outfile = args[i + 1]
                i += 2
            else:
                print("Error: -o requires a filename argument", file=sys.stderr)
                sys.exit(1)
        elif a.startswith("-"):
            i += 1
        else:
            infile = a
            i += 1

    process_serial_dump(infile, outfile)


if __name__ == "__main__":
    main()
