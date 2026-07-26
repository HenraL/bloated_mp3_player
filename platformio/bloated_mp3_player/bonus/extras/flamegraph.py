"""
Flamegraph renderer for Bloated MP3 Player profiling data.

Generates interactive SVG flamegraphs with dark theme, flame coloring,
search highlighting, and click-to-zoom.

Usage:
  python flamegraph.py < serial_dump.txt              # read from stdin
  python flamegraph.py serial_dump.txt                # single file
  python flamegraph.py --scan ./runs/                 # scan directory
  python flamegraph.py --scan ./runs/ -o out.svg      # scan + output name
  python flamegraph.py -h                             # help

Input format (output of Profiling::dump_traces / dump_task_stats):

  === PROFILING TRACES ===
  my_func 12345
  nested_func 6789
  === END PROFILING TRACES ===

  === PROFILING TASK STATS ===
  UI 2500
  Audio 1200
  === END PROFILING TASK STATS ===

Output: Interactive SVG flamegraph(s) with dark theme, flame colours,
hover tooltips, and click-to-zoom.
"""

import sys
import os
import re
import html
import glob
from collections import defaultdict

WIDTH = 1200
BAR_HEIGHT = 17
PADDING = 2
HEADER_HEIGHT = 80
FOOTER_HEIGHT = 30
MIN_BAR_WIDTH = 1
FONT_FAMILY = '"SF Mono","Monaco","Inconsolata","Fira Mono","Droid Sans Mono","Source Code Pro",monospace'


def print_help():
    h = """flamegraph.py — Interactive SVG flamegraph generator

Usage:
  python flamegraph.py < serial_dump.txt         Read from stdin
  python flamegraph.py serial_dump.txt           Process single file
  python flamegraph.py --scan <directory>        Scan directory for profiling dumps
  python flamegraph.py --scan <dir> -o out.svg   Scan + custom output basename
  python flamegraph.py -h                        Show this help

Options:
  -h, --help            Show this help message
  --scan <directory>    Recursively scan directory for .txt files containing
                        PROFILING sections and generate flamegraphs for each
  -o <filename>         Output SVG filename (default: flamegraph.svg)

Scan mode:
  When --scan is used, every .txt file under the given directory is checked
  for PROFILING sections. Each file that contains profiling data gets its
  own flamegraph named: <basename>_<section>.svg

Input format:
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
  python flamegraph.py --scan ../tmp/
  python flamegraph.py --scan ../tmp/ -o profile.svg
"""
    print(h)


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


def flamegraph_svg(entries, title, width=WIDTH):
    if not entries:
        return _empty_svg(title, "no data")
    total = sum(v for _, v in entries)
    if total == 0:
        return _empty_svg(title, "all zero")

    svg_w = width
    content_h = max(len(entries) * (BAR_HEIGHT + PADDING), 60)
    svg_h = HEADER_HEIGHT + content_h + FOOTER_HEIGHT

    base_id = title.replace(" ", "_").lower()
    grad_id = f"grad_{base_id}"
    lines = []
    _w = lines.append

    _w(f'<svg xmlns="http://www.w3.org/2000/svg" '
       f'width="{svg_w}" height="{svg_h}" '
       f'viewBox="0 0 {svg_w} {svg_h}"'
       f' style="background:#1a1a2e;font-family:{FONT_FAMILY};">')

    # -- defs: gradients + glow filter --
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

    # -- background --
    _w(f'<rect width="{svg_w}" height="{svg_h}" fill="url(#{grad_id})"/>')
    _w(f'<rect x="0" y="0" width="{svg_w}" height="{HEADER_HEIGHT}" '
       f'fill="#16213e" opacity="0.6"/>')
    _w(f'<line x1="0" y1="{HEADER_HEIGHT}" x2="{svg_w}" y2="{HEADER_HEIGHT}" '
       f'stroke="#0f3460" stroke-width="1"/>')

    # -- header --
    _w(f'<text x="20" y="26" font-size="15" font-weight="700" fill="#e94560">'
       f'{html.escape(title)}</text>')
    _w(f'<text x="20" y="48" font-size="11" fill="#8899aa">'
       f'total samples: {total:,}  |  {len(entries)} entries  |  '
       f'all values in microseconds</text>')

    _w(f'<text x="20" y="66" font-size="10" fill="#556677">'
       f'[hover for details]  [click to zoom]  '
       f'<tspan fill="#e94560">Ctrl+F</tspan> to search</text>')

    # -- bars --
    x_off = 10
    y_off = HEADER_HEIGHT + 4
    usable_w = svg_w - 20
    prev_name = None
    color_idx = 0

    entries_sorted = sorted(entries, key=lambda e: e[1], reverse=True)

    bar_data = []
    for rank, (name, val) in enumerate(entries_sorted):
        bw = max(val * usable_w / total, MIN_BAR_WIDTH)

        if name != prev_name:
            color_idx = (color_idx + 1) % 10
            prev_name = name

        pct = val * 100.0 / total
        t = pct / 100.0
        base_c, hi_c = flame_gradient(t)

        entry_id = f"b_{base_id}_{rank}"

        # tooltip
        tip = (f"{html.escape(name)}\\n"
               f"Duration: {val:,} us\\n"
               f"Share: {pct:.1f}%\\n"
               f"Rank: #{rank + 1}")

        bar_data.append((name, val, pct, bw, rank))

        _w(f'<g filter="url(#shadow)">')
        _w(f'<rect id="{entry_id}" class="frame"'
           f' x="{x_off:.1f}" y="{y_off}" width="{bw:.1f}"'
           f' height="{BAR_HEIGHT}" fill="{base_c}" rx="2.5" ry="2.5"'
           f' stroke="rgba(255,255,255,0.08)" stroke-width="0.5"'
           f' onclick="zoom_{base_id}({rank})"'
           f' onmouseover="hover_{base_id}(this,{rank})"'
           f' onmouseout="unhover_{base_id}(this,{rank})">'
           f'<title>{tip}</title>'
           f'</rect>')
        _w(f'<rect x="{x_off:.1f}" y="{y_off}" width="{min(bw,60):.1f}"'
           f' height="{BAR_HEIGHT}" fill="{hi_c}"'
           f' rx="2.5" ry="2.5" opacity="0.25"'
           f' style="pointer-events:none"/>'
           f'</g>')

        # label inside bar if wide enough
        label_w = max(val * usable_w / total, 0)
        digit_w = 6.5
        pct_lbl = f"{pct:.0f}%"
        label_text = f"{name} ({pct_lbl})"
        approx_w = len(label_text) * digit_w + 6

        if label_w >= approx_w:
            fc = "#1a1a2e" if t > 0.5 else "#ffffff"
            _w(f'<text x="{x_off + 4:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
               f' font-size="10" font-weight="500" fill="{fc}"'
               f' style="pointer-events:none;text-shadow:0 0 2px rgba(0,0,0,0.3);">'
               f'{html.escape(label_text)}</text>')
        elif label_w >= digit_w * 6:
            _w(f'<text x="{x_off + 3:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
               f' font-size="9" fill="#fff" opacity="0.7"'
               f' style="pointer-events:none;">{pct_lbl}</text>')

        x_off += bw
        if x_off - 10 >= usable_w:
            remaining = len(entries_sorted) - rank - 1
            if remaining > 0:
                _w(f'<text x="{svg_w - 200:.1f}" y="{y_off + BAR_HEIGHT - 4}"'
                   f' font-size="10" fill="#667" font-style="italic">'
                   f'… and {remaining} more</text>')
            break
        y_off += BAR_HEIGHT + PADDING

    # -- click-to-zoom javascript --
    _w('<script type="text/ecmascript"><![CDATA[')

    total_v = total
    _w(f'var total_{base_id} = {total_v};')
    _w(f'var data_{base_id} = [')
    for name, val, pct, bw, rank in bar_data:
        _w(f'  {{n:{json_quote(name)},v:{val},p:{pct:.1f}}},')
    _w('];')

    _w(f'''
var svg_{base_id} = document.querySelector("svg");
var orig_w_{base_id} = {svg_w};
var orig_h_{base_id} = {svg_h};
var zoomed_{base_id} = false;

function zoom_{base_id}(idx) {{
    var bars = document.querySelectorAll("#{base_id} .frame");
    if (zoomed_{base_id}) {{
        svg_{base_id}.setAttribute("viewBox", "0 0 " + orig_w_{base_id} + " " + orig_h_{base_id});
        zoomed_{base_id} = false;
        return;
    }}
    if (idx >= bars.length) return;
    var r = bars[idx];
    var x = parseFloat(r.getAttribute("x"));
    var w = parseFloat(r.getAttribute("width"));
    var y = parseFloat(r.getAttribute("y"));
    var pad = 10;
    var new_x = Math.max(0, x - pad);
    var new_w = Math.min(orig_w_{base_id}, w + 2 * pad);
    svg_{base_id}.setAttribute("viewBox", new_x + " " + Math.max(0,y-pad) + " " + new_w + " " + (orig_h_{base_id}));
    zoomed_{base_id} = true;
}}

function hover_{base_id}(el, idx) {{
    el.setAttribute("stroke", "#ffffff");
    el.setAttribute("stroke-width", "1.5");
    el.setAttribute("filter", "url(#glow)");
}}

function unhover_{base_id}(el, idx) {{
    el.setAttribute("stroke", "rgba(255,255,255,0.08)");
    el.setAttribute("stroke-width", "0.5");
    el.removeAttribute("filter");
}}
''')
    _w(']]></script>')

    # -- footer --
    _w(f'<text x="20" y="{svg_h - 10}" font-size="9" fill="#445566">'
       f'generated by bloated_mp3_player flamegraph.py  |  '
       f'click bar = zoom to frame  |  click again = reset</text>')

    _w('</svg>')
    return "\n".join(lines)


def json_quote(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"').replace("'", "\\'") + '"'


def _empty_svg(title, msg):
    return (f'<svg xmlns="http://www.w3.org/2000/svg" '
            f'width="{WIDTH}" height="100" '
            f'style="background:#1a1a2e;font-family:{FONT_FAMILY};">'
            f'<rect width="{WIDTH}" height="100" fill="#1a1a2e"/>'
            f'<text x="20" y="40" font-size="14" fill="#e94560" '
            f'font-weight="bold">{html.escape(title)}</text>'
            f'<text x="20" y="65" font-size="12" fill="#667">'
            f'{html.escape(msg)}</text></svg>')


def process_file(filepath, outfile_base):
    with open(filepath) as f:
        text = f.read()
    sections = parse_sections(text)
    if not sections:
        return 0
    for section_name, entries in sections.items():
        tag = section_name.replace("PROFILING ", "").lower().replace(" ", "_")
        fname = os.path.basename(filepath)
        base, _ = os.path.splitext(fname)
        out_name = f"{base}_{tag}.svg"
        svg = flamegraph_svg(entries, f"{fname} — {section_name}")
        with open(out_name, "w") as f:
            f.write(svg)
        print(f"  ✓ {out_name} ({len(entries)} entries)")
    return len(sections)


def process_stdin(outfile):
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


def process_file_single(filepath, outfile):
    with open(filepath) as f:
        text = f.read()
    sections = parse_sections(text)
    if not sections:
        print("No profiling sections found.", file=sys.stderr)
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


def scan_directory(directory, outfile_base):
    if not os.path.isdir(directory):
        print(f"Error: '{directory}' is not a directory.", file=sys.stderr)
        sys.exit(1)

    txt_files = []
    for root, dirs, files in os.walk(directory):
        for f in files:
            if f.endswith(".txt"):
                txt_files.append(os.path.join(root, f))

    if not txt_files:
        print(f"No .txt files found in '{directory}'.")
        return

    txt_files.sort()
    total_sections = 0
    total_files = 0

    print(f"Scanning {len(txt_files)} .txt files in '{directory}' ...\n")

    for fp in txt_files:
        sections = parse_sections(open(fp).read())
        if sections:
            total_files += 1
            for section_name, entries in sections.items():
                tag = section_name.replace("PROFILING ", "").lower().replace(" ", "_")
                fname = os.path.basename(fp)
                base, _ = os.path.splitext(fname)
                out_name = f"{base}_{tag}.svg"
                label = f"{fname} — {section_name}"
                svg = flamegraph_svg(entries, label)
                with open(out_name, "w") as f:
                    f.write(svg)
                print(f"  ✓ {out_name} ({len(entries)} entries)")
                total_sections += 1

    print(f"\nDone. {total_sections} flamegraph(s) from {total_files} file(s).")


def main():
    args = sys.argv[1:]

    # help
    if not args or "-h" in args or "--help" in args:
        print_help()
        sys.exit(0)

    # --scan <directory>
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
        scan_directory(directory, outfile_base)
        return

    # read from file or stdin
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

    if infile:
        process_file_single(infile, outfile)
    else:
        process_stdin(outfile)


if __name__ == "__main__":
    main()
