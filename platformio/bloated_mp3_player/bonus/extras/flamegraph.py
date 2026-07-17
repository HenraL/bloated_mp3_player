"""
Flamegraph renderer for Bloated MP3 Player profiling data.

Usage:
  python flamegraph.py < serial_dump.txt              # read from stdin
  python flamegraph.py serial_dump.txt                 # read from file
  python flamegraph.py serial_dump.txt -o output.svg   # specify output

Input format (output of Profiling::dump_traces / dump_task_stats):

  === PROFILING TRACES ===
  my_func 12345
  nested_func 6789
  === END PROFILING TRACES ===

  === PROFILING TASK STATS ===
  UI 2500
  Audio 1200
  === END PROFILING TASK STATS ===

Output: SVG flamegraph(s) showing CPU time / trace duration.
"""

import sys
import os
import re
import xml.etree.ElementTree as ET
from collections import defaultdict

PALETTE = [
    "#e74c3c", "#3498db", "#2ecc71", "#f39c12", "#9b59b6",
    "#1abc9c", "#e67e22", "#2980b9", "#27ae60", "#d35400",
    "#8e44ad", "#16a085", "#c0392b", "#2c3e50", "#f1c40f",
]

WIDTH = 1200
BAR_HEIGHT = 20
PADDING = 10
HEADER_HEIGHT = 40
LABEL_MIN_WIDTH = 6


def parse_sections(text):
    sections = {}
    pattern = r"=== (PROFILING \w+) ===\n(.*?)\n=== END \1 ==="
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


def flamegraph_svg(entries, title, width=WIDTH):
    if not entries:
        return f"<svg><text x='10' y='20'>{title}: no data</text></svg>"

    total = sum(v for _, v in entries)
    if total == 0:
        return f"<svg><text x='10' y='20'>{title}: all zero</text></svg>"

    max_depth = 1
    # Group identical names for stacking
    name_groups = defaultdict(list)
    for name, val in entries:
        name_groups[name].append(val)

    svg_w = width
    svg_h = HEADER_HEIGHT + (max_depth + 1) * (BAR_HEIGHT + PADDING)
    lines = []
    lines.append(f'<svg xmlns="http://www.w3.org/2000/svg" '
                 f'width="{svg_w}" height="{svg_h}" '
                 f'viewBox="0 0 {svg_w} {svg_h}">')
    lines.append(f'<text x="10" y="20" font-family="monospace" '
                 f'font-size="14" font-weight="bold">{title}</text>')
    lines.append(f'<text x="10" y="36" font-family="monospace" '
                 f'font-size="11" fill="#666">total samples: {total}  '
                 f'({len(entries)} entries)</text>')

    x_off = 0
    y_off = HEADER_HEIGHT
    prev_name = None
    color_idx = 0

    for name, val in entries:
        bw = max(val * svg_w / total, 0.5)

        if name != prev_name:
            color_idx = (color_idx + 1) % len(PALETTE)
            prev_name = name

        color = PALETTE[color_idx % len(PALETTE)]
        lines.append(
            f'<rect x="{x_off:.1f}" y="{y_off}" width="{bw:.1f}" '
            f'height="{BAR_HEIGHT}" fill="{color}" rx="2" ry="2">'
            f'<title>{name} ({val} us, {val*100/total:.1f}%)</title>'
            f'</rect>'
        )

        # Draw label inside rect if wide enough
        if bw >= LABEL_MIN_WIDTH:
            label = f"{name} ({val*100/total:.0f}%)"
            lines.append(
                f'<text x="{x_off + 3:.1f}" y="{y_off + BAR_HEIGHT - 5}" '
                f'font-family="monospace" font-size="10" fill="#fff" '
                f'text-anchor="start">{label}</text>'
            )

        x_off += bw
        if x_off > svg_w:
            break

    lines.append("</svg>")
    return "\n".join(lines)


def main():
    infile = None
    outfile = "flamegraph.svg"

    args = sys.argv[1:]
    if args and not args[0].startswith("-"):
        infile = args[0]
        args = args[1:]

    for i, a in enumerate(args):
        if a == "-o" and i + 1 < len(args):
            outfile = args[i + 1]
        elif a == "-o" and i == len(args) - 1:
            print("Error: -o requires a filename argument")
            sys.exit(1)

    if infile:
        with open(infile) as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    sections = parse_sections(text)
    if not sections:
        print("No profiling sections found in input.")
        sys.exit(1)

    for section_name, entries in sections.items():
        tag = section_name.replace("PROFILING ", "").lower()
        out_name = outfile
        if len(sections) > 1:
            base, ext = os.path.splitext(outfile)
            out_name = f"{base}_{tag}{ext}"

        svg = flamegraph_svg(entries, section_name)
        with open(out_name, "w") as f:
            f.write(svg)
        print(f"Wrote {out_name} ({len(entries)} entries)")

    if len(sections) == 0:
        print("No profiling data found.")


if __name__ == "__main__":
    main()
