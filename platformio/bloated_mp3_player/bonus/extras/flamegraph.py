"""
Flamegraph generator for Bloated MP3 Player profiling data.

Parses C++ serial dumps or source code, outputs Brendan Gregg folded format,
and can render interactive SVG flamegraphs.

Usage:
  python flamegraph.py -h                          help
  python flamegraph.py < serial_dump.txt           folded → stdout
  python flamegraph.py serial_dump.txt             folded → stdout
  python flamegraph.py --svg < serial_dump.txt     SVG → stdout
  python flamegraph.py --scan src/                 folded → stdout
  python flamegraph.py --scan src/ --svg           SVG → stdout
  python flamegraph.py --scan src/ -o out.svg      SVG → file

Input (serial dump):
  === PROFILING TRACES ===
  load_sd_card 45000
  === END PROFILING TRACES ===

Output (folded format — one line per sample):
  file.cpp;function_name;block_name 1
  load_sd_card 45000

SVG: Interactive flamegraph with dark theme, hover tooltips, click-to-zoom.

SVG rendering based on the canonical flamegraph visual style
(http://www.brendangregg.com/flamegraphs.html).
"""

import sys
import os
import re
import html
import math
import hashlib
from collections import defaultdict

# ────────────────────────────────────────────────────────────────────────────
# Constants
# ────────────────────────────────────────────────────────────────────────────

FRAME_HEIGHT = 16
FRAME_PAD = 1
HEADER_H = 70
FOOTER_H = 30
MARGIN_X = 10
MIN_FRAME_W = 1
FONT = 'monospace'


# ────────────────────────────────────────────────────────────────────────────
# Help
# ────────────────────────────────────────────────────────────────────────────

def print_help():
    print(r"""flamegraph.py — Flamegraph generator for C++ profiling data

Parse C++ serial dumps or source code and output folded-format data
or interactive SVG flamegraphs.

USAGE
  python flamegraph.py -h                          this help
  python flamegraph.py < serial_dump.txt           folded → stdout
  python flamegraph.py dump.txt                    folded → stdout
  python flamegraph.py --svg < dump.txt            SVG → stdout
  python flamegraph.py --scan src/                 folded → stdout
  python flamegraph.py --scan src/ --svg           SVG → stdout
  python flamegraph.py --scan src/ -o out.svg      SVG → file

MODES
  stdin / file        Read PROFILING sections from serial dump text
  --scan <dir>        Scan .cpp/.hpp files for PROFILE_BLOCK macros

OUTPUT FORMATS
  (default)           Brendan Gregg folded format — one stack per line:
                        frame;subframe;subsubframe count
  --svg               Interactive SVG flamegraph (dark theme, tooltips, zoom)

INPUT (serial dump)
  === PROFILING TRACES ===
  label_name  12345
  === END PROFILING TRACES ===

  === PROFILING TASK STATS ===
  TaskName  2500
  === END PROFILING TASK STATS ===

EXAMPLES
  python flamegraph.py < serial_dump.txt
  python flamegraph.py --svg < serial_dump.txt > profile.svg
  python flamegraph.py --scan ../src/ --svg > code_profile.svg
  python flamegraph.py --scan ../src/ -o out.svg""")
    sys.exit(0)


# ────────────────────────────────────────────────────────────────────────────
# Serial-dump parsing
# ────────────────────────────────────────────────────────────────────────────

def parse_sections(text):
    sections = {}
    pat = r"=== (PROFILING [\w ]+) ===\n(.*?)\n=== END \1 ==="
    for m in re.finditer(pat, text, re.DOTALL):
        name = m.group(1)
        entries = []
        for line in m.group(2).strip().splitlines():
            parts = line.rsplit(None, 1)
            if len(parts) == 2:
                try:
                    entries.append((parts[0], int(parts[1])))
                except ValueError:
                    pass
        sections[name] = entries
    return sections


# ────────────────────────────────────────────────────────────────────────────
# C++ source parsing
# ────────────────────────────────────────────────────────────────────────────

PROFILE_BLOCK_RE = re.compile(r'PROFILE_BLOCK\s*\(\s*"([^"]*)"\s*\)')
PROFILE_BEGIN_RE = re.compile(r'PROFILE_BEGIN\s*\(\s*"([^"]*)"\s*\)')
PROFILE_END_RE = re.compile(r'PROFILE_END\s*\(\s*"([^"]*)"\s*\)')

FUNC_SIG_RE = re.compile(
    r'(?:[\w:~\*&<>,\s]+?\s+)?'
    r'(\w+)'
    r'\s*\([^)]*\)'
    r'\s*(?:const\s*)?'
    r'\s*(?:override\s*)?'
    r'\s*$'
)

KEYWORDS = frozenset({
    'if', 'while', 'for', 'switch', 'catch', 'else',
    'return', 'delete', 'sizeof', 'throw', 'try',
})


def strip_cpp_comments(text):
    out = []
    i = 0
    n = len(text)
    in_block = False
    in_line = False
    in_str = False
    sq = None
    while i < n:
        c = text[i]
        if in_str:
            out.append(c)
            if c == '\\' and i + 1 < n:
                out.append(text[i + 1])
                i += 2
                continue
            if c == sq:
                in_str = False
            i += 1
            continue
        if c in ('"', "'"):
            in_str = True
            sq = c
            out.append(c)
            i += 1
            continue
        if not in_block and c == '/' and i + 1 < n and text[i + 1] == '/':
            in_line = True
            i += 2
            continue
        if not in_block and c == '/' and i + 1 < n and text[i + 1] == '*':
            in_block = True
            i += 2
            continue
        if in_line:
            if c == '\n':
                in_line = False
                out.append(c)
            i += 1
            continue
        if in_block:
            if c == '*' and i + 1 < n and text[i + 1] == '/':
                in_block = False
                i += 2
                continue
            i += 1
            continue
        out.append(c)
        i += 1
    return ''.join(out)


def find_functions(clean):
    lines = clean.splitlines()
    funcs = []
    pending = None
    pending_li = -1
    for li, line in enumerate(lines):
        s = line.strip()
        if not s or s.startswith('#'):
            continue
        if '{' in s:
            if pending is not None:
                funcs.append((pending, pending_li))
                pending = None
                pending_li = -1
                continue
            bare = s.split('{')[0].strip()
            m = FUNC_SIG_RE.match(bare)
            if m and m.group(1) not in KEYWORDS:
                funcs.append((m.group(1), li))
            continue
        m = FUNC_SIG_RE.match(s)
        if m and m.group(1) not in KEYWORDS:
            pending = m.group(1)
            pending_li = li
        else:
            pending = None
            pending_li = -1
    return funcs


def enclosing_function(lineno, funcs):
    best, best_li = None, -1
    for name, fl in funcs:
        if fl <= lineno and fl > best_li:
            best, best_li = name, fl
    return best or '(global)'


def parse_source_file(path):
    with open(path) as f:
        raw = f.read()
    clean = strip_cpp_comments(raw)
    funcs = find_functions(clean)
    lines = clean.splitlines()
    blocks = []
    for li, line in enumerate(lines):
        for m in PROFILE_BLOCK_RE.finditer(line):
            blocks.append((m.group(1), li, enclosing_function(li, funcs)))
        for m in PROFILE_BEGIN_RE.finditer(line):
            blocks.append(('BEGIN:' + m.group(1), li, enclosing_function(li, funcs)))
    rel = os.path.relpath(path)
    return {'file': rel, 'blocks': blocks, 'functions': funcs}


def scan_source_tree(root):
    results = []
    for dirpath, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        for fn in files:
            if fn.endswith(('.cpp', '.hpp')):
                try:
                    results.append(parse_source_file(os.path.join(dirpath, fn)))
                except Exception as e:
                    print(f'  ! {fn}: {e}', file=sys.stderr)
    return results


# ────────────────────────────────────────────────────────────────────────────
# Folded-format output
# ────────────────────────────────────────────────────────────────────────────

def emit_folded(entries, title=''):
    if title:
        pass
    for name, val in entries:
        print(f'{name} {val}')


def emit_source_folded(parsed, title=''):
    if title:
        pass
    for pf in parsed:
        f = pf['file']
        for bname, lineno, func in pf['blocks']:
            print(f'{f};{func};{bname} 1')


# ────────────────────────────────────────────────────────────────────────────
# SVG flamegraph rendering
# SVG rendering based on the canonical flamegraph visual style
# (http://www.brendangregg.com/flamegraphs.html).
# Colour scheme inspired by Brendan Gregg's FlameGraph tools.
# ────────────────────────────────────────────────────────────────────────────

def name_hash(name):
    h = hashlib.md5(name.encode()).digest()
    r = h[0] % 180 + 50
    g = h[1] % 160 + 60
    b = h[2] % 140 + 80
    return f'#{r:02x}{g:02x}{b:02x}'


def name_hash_hot(name):
    h = hashlib.md5(name.encode()).digest()
    t = (h[0] + h[1]) / 510.0
    if t < 0.33:
        r = 140 + int(100 * t / 0.33)
        g = 30 + int(40 * t / 0.33)
        b = 20
    elif t < 0.66:
        s = (t - 0.33) / 0.33
        r = 240 + int(15 * s)
        g = 70 + int(170 * s)
        b = 20 + int(30 * s)
    else:
        s = (t - 0.66) / 0.34
        r = 255
        g = 240 + int(15 * s)
        b = 50 + int(200 * s)
    return f'#{min(r,255):02x}{min(g,255):02x}{min(b,255):02x}'


def render_folded_to_svg(folded, title='Flamegraph', width=1200):
    if not folded:
        svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}"'
               f' height="100" style="background:#1a1a2e" font-family="{FONT}">'
               f'<rect width="{width}" height="100" fill="#1a1a2e"/>'
               f'<text x="20" y="40" font-size="14" fill="#e94560"'
               f' font-weight="bold">no data</text></svg>')
        return svg

    total = sum(v for v in folded.values())
    if total == 0:
        svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}"'
               f' height="100" style="background:#1a1a2e" font-family="{FONT}">'
               f'<rect width="{width}" height="100" fill="#1a1a2e"/>'
               f'<text x="20" y="40" font-size="14" fill="#e94560"'
               f' font-weight="bold">all zero</text></svg>')
        return svg

    frames = []
    for stack, count in folded.items():
        funcs = stack.split(';')
        for depth, fname in enumerate(funcs):
            frames.append({
                'name': fname,
                'stack': stack,
                'depth': depth,
                'count': count,
                'label': ';'.join(funcs[:depth + 1]),
            })

    merged = defaultdict(int)
    for f in frames:
        key = (f['depth'], f['label'])
        merged[key] += f['count']

    levels = defaultdict(list)
    for (depth, label), count in merged.items():
        levels[depth].append({'label': label, 'count': count, 'depth': depth})

    max_depth = max(levels.keys()) if levels else 0
    usable_w = width - 2 * MARGIN_X
    content_h = (max_depth + 1) * (FRAME_HEIGHT + FRAME_PAD) + 10
    svg_h = HEADER_H + content_h + FOOTER_H

    lines = []
    _w = lines.append
    tid = 'fg'

    _w(f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}"'
       f' height="{svg_h}" viewBox="0 0 {width} {svg_h}"'
       f' style="background:#1a1a2e" font-family="{FONT}">')

    _w('<defs>'
       '<linearGradient id="bg" x1="0" y1="0" x2="0" y2="1">'
       '<stop offset="0%" stop-color="#2d2d5e"/>'
       '<stop offset="100%" stop-color="#1a1a2e"/>'
       '</linearGradient>'
       '<filter id="sh"><feDropShadow dx="0" dy="1" stdDeviation="1"'
       ' flood-opacity="0.3"/></filter>'
       '</defs>')

    _w(f'<rect width="{width}" height="{svg_h}" fill="url(#bg)"/>')
    _w(f'<rect x="0" y="0" width="{width}" height="{HEADER_H}"'
       f' fill="#16213e" opacity="0.6"/>')
    _w(f'<line x1="0" y1="{HEADER_H}" x2="{width}" y2="{HEADER_H}"'
       f' stroke="#0f3460" stroke-width="1"/>')

    _w(f'<text x="20" y="24" font-size="15" font-weight="700" fill="#e94560">'
       f'{html.escape(title)}</text>')
    _w(f'<text x="20" y="44" font-size="11" fill="#8899aa">'
       f'{total} samples  |  {len(folded)} unique stacks  |  '
       f'{max_depth + 1} depth levels</text>')
    _w(f'<text x="20" y="60" font-size="10" fill="#556677">'
       f'[hover for details]  [click to zoom]</text>')

    y0 = HEADER_H + 5
    zid = tid

    _w(f'<script type="text/ecmascript"><![CDATA['
       f'var z_{zid}=false,ow={width},oh={svg_h};'
       f'function zf(idx){{'
       f'var e=document.getElementById("f_"+idx);if(!e)return;'
       f'if(z_{zid}){{z_{zid}=false;'
       f'document.querySelector("svg").setAttribute("viewBox","0 0 "+ow+" "+oh);'
       f'return}}'
       f'var x=parseFloat(e.getAttribute("x")),'
       f'w=parseFloat(e.getAttribute("width")),'
       f'y=parseFloat(e.getAttribute("y"));'
       f'document.querySelector("svg").setAttribute("viewBox",'
       f'Math.max(0,x-10)+" "+Math.max(0,y-10)+" "+'
       f'Math.min(ow,w+20)+" "+oh);z_{zid}=true}}'
       f'function hl(e){{e.setAttribute("stroke","#fff");'
       f'e.setAttribute("stroke-width","1.5")}}'
       f'function uhl(e){{e.setAttribute("stroke","rgba(255,255,255,0.08)");'
       f'e.setAttribute("stroke-width","0.5")}}'
       f']]></script>')

    fidx = 0
    for depth in sorted(levels):
        y = y0 + depth * (FRAME_HEIGHT + FRAME_PAD)
        sorted_frames = sorted(levels[depth], key=lambda x: -x['count'])
        x = MARGIN_X
        for fdata in sorted_frames:
            w = max(fdata['count'] * usable_w / total, MIN_FRAME_W)
            color = name_hash_hot(fdata['label'])
            name_short = fdata['label'].split(';')[-1]
            pct = fdata['count'] * 100.0 / total
            tip = (f'{html.escape(fdata["label"])}\n'
                   f'{fdata["count"]} samples ({pct:.1f}%)')

            _w(f'<g filter="url(#sh)">'
               f'<rect id="f_{fidx}" class="f"'
               f' x="{x:.1f}" y="{y}" width="{w:.1f}"'
               f' height="{FRAME_HEIGHT}" fill="{color}" rx="1.5" ry="1.5"'
               f' stroke="rgba(255,255,255,0.08)" stroke-width="0.5"'
               f' onclick="zf({fidx})"'
               f' onmouseover="hl(this)" onmouseout="uhl(this)">'
               f'<title>{tip}</title></rect></g>')

            label = f'{name_short} ({pct:.0f}%)'
            lbl_w = len(label) * 6.5 + 4
            if w >= lbl_w:
                _w(f'<text x="{x + 3:.1f}" y="{y + FRAME_HEIGHT - 4}"'
                   f' font-size="9" fill="#fff"'
                   f' style="pointer-events:none">'
                   f'{html.escape(label)}</text>')
            elif w >= 35:
                _w(f'<text x="{x + 2:.1f}" y="{y + FRAME_HEIGHT - 4}"'
                   f' font-size="8" fill="#fff" opacity="0.7"'
                   f' style="pointer-events:none">{pct:.0f}%</text>')

            x += w
            fidx += 1

    _w(f'<text x="20" y="{svg_h - 10}" font-size="9" fill="#445566">'
       f'click = zoom  |  click again = reset  |  '
       f'generated by bloated_mp3_player flamegraph.py</text>')
    _w('</svg>')
    return '\n'.join(lines)


def build_folded_from_entries(entries):
    folded = defaultdict(int)
    for name, val in entries:
        folded[name] += val
    return folded


def build_folded_from_source(parsed):
    folded = defaultdict(int)
    for pf in parsed:
        f = pf['file']
        for bname, lineno, func in pf['blocks']:
            stack = f'{f};{func};{bname}'
            folded[stack] += 1
    return folded


# ────────────────────────────────────────────────────────────────────────────
# Main
# ────────────────────────────────────────────────────────────────────────────

def main():
    args = sys.argv[1:]

    if '-h' in args or '--help' in args:
        print_help()

    if not args and sys.stdin.isatty():
        print_help()

    svg_mode = '--svg' in args
    scan_mode = '--scan' in args
    outfile = None

    tail = []
    skip_next = False
    for i, a in enumerate(args):
        if skip_next:
            skip_next = False
            continue
        if a == '--svg':
            continue
        if a == '--scan':
            scan_mode = True
            continue
        if a == '-o':
            if i + 1 < len(args):
                outfile = args[i + 1]
                skip_next = True
            continue
        if a.startswith('-'):
            continue
        tail.append(a)

    if scan_mode:
        d = tail[0] if tail else '.'
        parsed = scan_source_tree(d)
        total_blocks = sum(len(p['blocks']) for p in parsed)
        if total_blocks == 0:
            print(f'No PROFILE_BLOCK macros found in {d}.', file=sys.stderr)
            sys.exit(1)
        if svg_mode or outfile:
            folded = build_folded_from_source(parsed)
            title = f'Source Profile — {d}'
            svg = render_folded_to_svg(folded, title)
            if outfile:
                with open(outfile, 'w') as f:
                    f.write(svg)
                print(f'Wrote {outfile}', file=sys.stderr)
            else:
                print(svg)
        else:
            for pf in parsed:
                f = pf['file']
                for bname, lineno, func in pf['blocks']:
                    print(f'{f};{func};{bname} 1')
        return

    infile = tail[0] if tail else None
    text = ''
    if infile:
        with open(infile) as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    sections = parse_sections(text)
    if not sections:
        print('No PROFILING sections found in input.', file=sys.stderr)
        sys.exit(1)

    for sname, entries in sections.items():
        tag = sname.replace('PROFILING ', '').lower().replace(' ', '_')
        if svg_mode or outfile:
            folded = build_folded_from_entries(entries)
            suffix = f'_{tag}' if len(sections) > 1 else ''
            title = sname
            svg = render_folded_to_svg(folded, title)
            if outfile:
                base, ext = os.path.splitext(outfile)
                fname = f'{base}{suffix}{ext}'
                with open(fname, 'w') as f:
                    f.write(svg)
                print(f'Wrote {fname}', file=sys.stderr)
            else:
                print(svg)
        else:
            emit_folded(entries)


if __name__ == '__main__':
    main()
