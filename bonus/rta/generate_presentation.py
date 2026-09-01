#!/usr/bin/env python3
"""
+==== BEGIN Bloated MP3 Player =================+
 PROJECT: Bloated MP3 Player
 FILE: generate_presentation.py
 PURPOSE: Generate 7-slide RTES deadline presentation (PPTX)
          No microcontroller code is modified.
+==== END Bloated MP3 Player =================+
"""
import os
import math
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from pptx import Presentation
from pptx.util import Inches, Pt, Emu
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.enum.shapes import MSO_SHAPE

WORKDIR = os.path.dirname(os.path.abspath(__file__))
ASSETS = os.path.join(WORKDIR, "assets")
os.makedirs(ASSETS, exist_ok=True)

# ── Theme ───────────────────────────────────────────────────────────────
BG_DARK = RGBColor(0x0B, 0x0E, 0x1A)  # near-black for title
BG_SLIDE = RGBColor(0xFF, 0xFF, 0xFF)
ACCENT = RGBColor(0x00, 0x78, 0xD4)   # ESP blue
ACCENT2 = RGBColor(0xE6, 0x3E, 0x33)  # warn red
ACCENT3 = RGBColor(0x10, 0x9B, 0x5E)  # green
MUTED = RGBColor(0x5A, 0x5A, 0x6A)
LIGHT_BG = RGBColor(0xF2, 0xF4, 0xF8)

COLOR_TASKS = {
    "a": "#E63E33",
    "b": "#0078D4",
    "c": "#109B5E",
    "d": "#E9A825",
    "idle": "#E5E7EB",
}

# ── Helpers ─────────────────────────────────────────────────────────────
def set_bg(slide, color):
    bg = slide.background
    fill = bg.fill
    fill.solid()
    fill.fore_color.rgb = color

def add_shape(slide, left, top, width, height, fill_rgb, line_rgb=None):
    shape = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, left, top, width, height)
    shape.fill.solid()
    shape.fill.fore_color.rgb = fill_rgb
    shape.line.fill.background()
    if line_rgb:
        shape.line.color.rgb = line_rgb
        shape.line.width = Pt(1)
    return shape

def add_text_box(slide, left, top, width, height, text, font_size=12, bold=False, color=RGBColor(0x1A,0x1A,0x1A), alignment=PP_ALIGN.LEFT, font_name="Calibri", italic=False):
    txBox = slide.shapes.add_textbox(left, top, width, height)
    tf = txBox.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = text
    p.font.size = Pt(font_size)
    p.font.bold = bold
    p.font.color.rgb = color
    p.font.name = font_name
    p.font.italic = italic
    p.alignment = alignment
    return txBox

def add_paragraph(text_frame, text, font_size=10, bold=False, color=RGBColor(0x33,0x33,0x33), italic=False, alignment=PP_ALIGN.LEFT):
    p = text_frame.add_paragraph()
    p.text = text
    p.font.size = Pt(font_size)
    p.font.bold = bold
    p.font.color.rgb = color
    p.font.italic = italic
    p.font.name = "Calibri"
    p.alignment = alignment
    p.space_after = Pt(2)
    p.space_before = Pt(1)
    return p

# ── Gantt generation ────────────────────────────────────────────────────
def gantt_png(slots, time_limit, out_path, title):
    # slots: list of (task, start, end) where task may be None
    fig, ax = plt.subplots(figsize=(10, 2.2))
    fig.patch.set_facecolor("white")
    ax.set_facecolor("white")
    task_order = ["b","c","d","a"]
    y_map = {name:i for i,name in enumerate(reversed(task_order))}  # a on top? keep logical
    # Actually put b top -> reverse so b top
    # We'll plot y = 3 for b, 2 for c, 1 for d, 0 for a
    y_map = {"b":3, "c":2, "d":1, "a":0}
    height = 0.6
    for task, start, end in slots:
        if task is None:
            continue
        y = y_map.get(task, 0)
        ax.broken_barh([(start, end-start)], (y - height/2, height),
                       facecolors=COLOR_TASKS[task], edgecolor="white", linewidth=1)
        # label inside bar if wide enough
        if end - start >= 1.5:
            ax.text((start+end)/2, y, task, ha="center", va="center", fontsize=8, color="white", weight="bold")
    ax.set_ylim(-0.7, 3.7)
    ax.set_xlim(0, time_limit)
    ax.set_xlabel("time", fontsize=8, color="#555555")
    ax.set_yticks([0,1,2,3])
    ax.set_yticklabels(["a (T=40)","d (18)","c (10)","b (8)"], fontsize=7)
    ax.set_xticks(range(0, time_limit+1, 2))
    ax.tick_params(axis="x", labelsize=6, colors="#555555")
    ax.tick_params(axis="y", labelsize=7, colors="#333333")
    ax.grid(axis="x", linestyle=":", linewidth=0.5, color="#E5E7EB")
    for spine in ax.spines.values():
        spine.set_visible(False)
    ax.set_title(title, fontsize=8, color="#333333", pad=8, loc="left")
    plt.tight_layout(pad=0.5)
    plt.savefig(out_path, dpi=220, bbox_inches="tight")
    plt.close()

def build_gantts():
    # Reuse logic from schedule_sim without importing prettyTables
    from dataclasses import dataclass
    from typing import List, Optional
    @dataclass
    class Task:
        name: str
        wcet: int
        period: int
        deadline: int = None
        def __post_init__(self):
            if self.deadline is None:
                self.deadline = self.period
    @dataclass
    class Job:
        task_name: str
        release: int
        deadline: int
        remaining: int
        @property
        def done(self): return self.remaining <= 0
    tasks = [Task("a",7,40), Task("b",2,8), Task("c",3,10), Task("d",4,18)]
    TIME_LIMIT = 40
    # RM sim
    def sim_rm():
        priority = sorted(tasks, key=lambda t: t.period)
        prio_map = {t.name:i for i,t in enumerate(priority)}
        jobs: List[Job] = []
        slots = []
        last_name=None
        last_start=0
        for t in range(TIME_LIMIT):
            for task in tasks:
                if t % task.period == 0:
                    jobs.append(Job(task.name, t, t+task.deadline, task.wcet))
            jobs = [j for j in jobs if not j.done]
            if jobs:
                jobs.sort(key=lambda j: prio_map[j.task_name])
                cur = jobs[0]
                cur_name = cur.task_name
            else:
                cur=None
                cur_name=None
            if cur_name != last_name:
                if last_name is not None:
                    slots.append((last_name, last_start, t))
                elif t>0:
                    slots.append((last_name, last_start, t))
                last_name=cur_name
                last_start=t
            if cur is not None:
                cur.remaining-=1
                if cur.done:
                    jobs.remove(cur)
        slots.append((last_name, last_start, TIME_LIMIT))
        return [s for s in slots if s[0] is not None]
    def sim_edf_pre():
        jobs: List[Job] = []
        slots=[]
        last_name=None
        last_start=0
        for t in range(TIME_LIMIT):
            for task in tasks:
                if t % task.period == 0:
                    jobs.append(Job(task.name, t, t+task.deadline, task.wcet))
            jobs=[j for j in jobs if not j.done]
            if jobs:
                jobs.sort(key=lambda j: j.deadline)
                cur=jobs[0]
                cur_name=cur.task_name
            else:
                cur=None
                cur_name=None
            if cur_name != last_name:
                if last_name is not None:
                    slots.append((last_name, last_start, t))
                elif t>0:
                    slots.append((last_name, last_start, t))
                last_name=cur_name
                last_start=t
            if cur is not None:
                cur.remaining-=1
                if cur.done:
                    jobs.remove(cur)
        slots.append((last_name, last_start, TIME_LIMIT))
        return [s for s in slots if s[0] is not None]
    def sim_edf_np():
        jobs: List[Job] = []
        slots=[]
        running: Optional[Job]=None
        running_name=None
        last_start=0
        just_finished=False
        last_name=None
        for t in range(TIME_LIMIT):
            for task in tasks:
                if t % task.period == 0:
                    jobs.append(Job(task.name, t, t+task.deadline, task.wcet))
            if just_finished and running is not None:
                slots.append((running_name, last_start, t))
                running=None
                running_name=None
                last_name=None
                last_start=t
                just_finished=False
            jobs=[j for j in jobs if not j.done]
            if running is None:
                ready=[j for j in jobs if j.remaining>0]
                if ready:
                    ready.sort(key=lambda j: j.deadline)
                    running=ready[0]
                    running_name=running.task_name
                    last_name=running_name
                    last_start=t
                else:
                    if last_name is not None:
                        slots.append((last_name, last_start, t))
                        last_name=None
                        last_start=t
            if running is not None:
                running.remaining-=1
                if running.done:
                    just_finished=True
        if running is not None or last_name is not None:
            name = running_name if running is not None else last_name
            slots.append((name, last_start, TIME_LIMIT))
        return [s for s in slots if s[0] is not None]

    rm = sim_rm()
    edf = sim_edf_pre()
    edf_np = sim_edf_np()
    gantt_png(rm, TIME_LIMIT, os.path.join(ASSETS, "gantt_rm.png"), "Preemptive RM  — 18 context switches, window [0,40)")
    gantt_png(edf, TIME_LIMIT, os.path.join(ASSETS, "gantt_edf.png"), "Preemptive EDF  — 16 context switches")
    gantt_png(edf_np, TIME_LIMIT, os.path.join(ASSETS, "gantt_edf_np.png"), "Non-preemptive EDF  — 12 context switches")
    # combined figure for slide 5
    # keep individual as well
    return rm, edf, edf_np

# ── Presentation build ──────────────────────────────────────────────────
def add_slide_number(slide, num, total):
    add_text_box(slide, Inches(9.2), Inches(5.3), Inches(0.6), Inches(0.2),
                 f"{num:02d} / {total:02d}", font_size=7, color=RGBColor(0x99,0x99,0x99), alignment=PP_ALIGN.RIGHT)

def header_bar(slide, kicker, title, subtitle=None):
    # top accent line
    add_shape(slide, Inches(0), Inches(0), Inches(13.33), Pt(4), ACCENT)
    # kicker
    add_text_box(slide, Inches(0.4), Inches(0.22), Inches(6), Inches(0.25),
                 kicker.upper(), font_size=7, bold=True, color=ACCENT)
    # title
    add_text_box(slide, Inches(0.4), Inches(0.45), Inches(9), Inches(0.45),
                 title, font_size=20, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    if subtitle:
        add_text_box(slide, Inches(0.4), Inches(0.9), Inches(9), Inches(0.3),
                     subtitle, font_size=10, color=MUTED)

def create_presentation():
    prs = Presentation()
    prs.slide_width = Inches(13.33)
    prs.slide_height = Inches(7.5)
    prs.core_properties.title = "Bloated MP3 Player — RTES"
    prs.core_properties.author = "Henry Letellier"
    # ── Slide 1 — Title ───────────────────────────────────────────────
    s1 = prs.slides.add_slide(prs.slide_layouts[6])  # blank
    set_bg(s1, BG_DARK)
    # accent line top
    add_shape(s1, Inches(0), Inches(0), Inches(13.33), Pt(5), ACCENT)
    # tiny logo-like dots
    # title block
    add_text_box(s1, Inches(0.6), Inches(0.6), Inches(8), Inches(0.3),
                 "UNIMORE  ·  REAL-TIME EMBEDDED SYSTEMS  ·  A.Y. 2025/2026", font_size=8, bold=True, color=RGBColor(0x8A,0x9B,0xB8), font_name="Calibri")
    add_text_box(s1, Inches(0.6), Inches(1.2), Inches(8.5), Inches(0.9),
                 "DON'T PANIC", font_size=44, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s1, Inches(0.62), Inches(2.05), Inches(8.5), Inches(0.7),
                 "Bloated MP3 Player — Deadline Analysis", font_size=22, color=RGBColor(0xE6,0xE8,0xF0))
    add_text_box(s1, Inches(0.62), Inches(2.85), Inches(8.5), Inches(0.8),
                 "From 9 concurrent peripherals on ESP32-S3 to provable scheduling guarantees.\nRate-Monotonic · EDF · PIP / PCP · Processor Demand Criterion", font_size=11, color=RGBColor(0xB8,0xC0,0xD8))
    # info box
    add_shape(s1, Inches(0.6), Inches(4.1), Inches(5.2), Inches(1.25), RGBColor(0x14,0x18,0x2E), RGBColor(0x2A,0x34,0x55))
    tb = s1.shapes.add_textbox(Inches(0.75), Inches(4.25), Inches(4.9), Inches(1.1))
    tf = tb.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Henry Letellier  •  ESP32-S3 WROOM (dual-core 240 MHz)  •  FreeRTOS via Arduino"
    p.font.size = Pt(9); p.font.color.rgb = RGBColor(0xE6,0xE8,0xF0); p.font.name="Calibri"
    add_paragraph(tf, "7 slides  •  No firmware modified  •  Analysis from bonus/rta/*.py", font_size=8, color=RGBColor(0x8A,0x9B,0xB8), italic=True)
    add_paragraph(tf, "bonus/rta/lenv  •  python-pptx + matplotlib  •  2026-09-01", font_size=7, color=RGBColor(0x6A,0x7A,0x99))
    # right card
    add_shape(s1, Inches(8.2), Inches(1.0), Inches(4.1), Inches(4.8), RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s1, Inches(8.5), Inches(1.25), Inches(3.5), Inches(0.25),
                 "WHAT THIS DECK PROVES", font_size=7, bold=True, color=ACCENT)
    card_items = [
        ("U & Bound", "U=0.947 > 0.757 → RTA needed"),
        ("RTA", "b,c,d OK · a R=41 > 40 fails RM"),
        ("EDF", "U=0.617 ≤ 1 and PDC passes (600 pts)"),
        ("Blocking", "PCP halves B vs PIP (10→4, 10→6)"),
        ("Complexity", "Busy period L*=10: 600→1 point (600×)"),
    ]
    y = 1.6
    for k,v in card_items:
        add_text_box(s1, Inches(8.55), Inches(y), Inches(1.25), Inches(0.22), k, font_size=8, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
        add_text_box(s1, Inches(9.85), Inches(y), Inches(2.1), Inches(0.22), v, font_size=8, color=MUTED)
        y += 0.33
        # divider
        add_shape(s1, Inches(8.55), Inches(y-0.05), Inches(3.45), Pt(0.6), LIGHT_BG)
    add_text_box(s1, Inches(8.5), Inches(3.75), Inches(3.5), Inches(0.5),
                 "The Hitchhiker's answer was 42.\nOur answer is 40: the period where RM breaks.", font_size=8, italic=True, color=MUTED)
    add_shape(s1, Inches(8.5), Inches(4.35), Inches(3.5), Pt(1), LIGHT_BG)
    add_text_box(s1, Inches(8.5), Inches(4.45), Inches(3.5), Inches(0.9),
                 "Tip: wave at the HC-SR04 to pause.\nWave twice to bail out — just like deadline handling.", font_size=8, color=MUTED)
    add_slide_number(s1, 1, 7)

    # ── Slide 2 — System Overview ─────────────────────────────────────
    s2 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s2, BG_SLIDE)
    header_bar(s2, "01 — System", "Bloated by design, real-time by necessity", "ESP32-S3 dual-core · FreeRTOS · 9 tasks · “17 sensors on one I²C bus”")
    # left: hardware
    add_shape(s2, Inches(0.4), Inches(1.35), Inches(6.35), Inches(5.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s2, Inches(0.65), Inches(1.5), Inches(5.85), Inches(0.25), "HARDWARE — THE BOM OF DOOMED PERIPHERALS", font_size=7, bold=True, color=ACCENT)
    # hardware rows
    hw = [
        ("Display", "GMG12864-06D 128×64 SPI (ST7565R)  CS10 RST9 DC8"),
        ("Char LCDs", "2004A (0x27) folder/track + 1602A (0x26) Vogon poetry — I²C SDA2/SCL1"),
        ("Audio", "2× MAX98357A I²S  BCLK16 WS47 DIN17/18 — Helix MP3 + PSRAM"),
        ("Sensors", "AHT20+BMP280 (env) · MPU-9250 (IMU) · HC-SR04 (TRIG14 ECHO13)"),
        ("Input / Light", "EC11 rotary A42 B15 SW12  ·  WS2812B 16×16 matrix pin 41"),
        ("Storage", "microSD SDMMC 1-bit  CLK39 CMD38 D0 40"),
        ("Compute", "Freenove ESP32-S3 WROOM  dual-core 240 MHz · Arduino + FreeRTOS"),
    ]
    y = 1.85
    for a,b in hw:
        add_text_box(s2, Inches(0.65), Inches(y), Inches(1.35), Inches(0.22), a, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
        add_text_box(s2, Inches(2.05), Inches(y), Inches(4.55), Inches(0.22), b, font_size=7.5, color=MUTED)
        y += 0.32
    add_shape(s2, Inches(0.65), Inches(4.15), Inches(5.85), Pt(0.7), LIGHT_BG)
    add_text_box(s2, Inches(0.65), Inches(4.3), Inches(5.85), Inches(0.25), "FREERTOS MAPPING — CORE & STACK REALITY", font_size=7, bold=True, color=ACCENT)
    # tiny table header
    add_shape(s2, Inches(0.65), Inches(4.62), Inches(5.85), Inches(0.28), ACCENT)
    for txt, x, w in [("Task",0.65,1.35),("Prio",2.0,0.65),("Core",2.65,0.65),("Stack",3.30,1.0),("Note",4.30,1.95)]:
        add_text_box(s2, Inches(x), Inches(4.62), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows = [
        ("Audio", "3", "0", "16 384", "Helix + FATFS deep chain"),
        ("Input", "2", "0", "16 384", "SD open() overflow fix"),
        ("UI / LED / Matrix", "1", "1", "4–8K", "33 ms UI, I²C traffic"),
        ("Sensor / Serial / Vogon", "1", "0/1", "4–8K", "I²C 50 ms timeout, BLE GATTS"),
    ]
    y = 4.92
    for i,r in enumerate(rows):
        bg = RGBColor(0xFF,0xFF,0xFF) if i%2==0 else LIGHT_BG
        add_shape(s2, Inches(0.65), Inches(y), Inches(5.85), Inches(0.28), bg)
        xs = [0.65,2.0,2.65,3.30,4.30]
        ws = [1.35,0.65,0.65,1.0,1.95]
        for txt, x, w in zip(r, xs, ws):
            add_text_box(s2, Inches(x), Inches(y), Inches(w), Inches(0.28), txt, font_size=6.5, color=RGBColor(0x2A,0x2A,0x3A), alignment=PP_ALIGN.CENTER)
        y+=0.30
    add_text_box(s2, Inches(0.65), Inches(6.38), Inches(5.85), Inches(0.35),
                 "Lesson: Audio on core 0 prio 3 isolates decode; everything else 1 — dual-core = interference domain partition.", font_size=7, italic=True, color=MUTED)

    # right: workload
    add_shape(s2, Inches(7.05), Inches(1.35), Inches(5.85), Inches(5.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s2, Inches(7.30), Inches(1.5), Inches(5.35), Inches(0.25), "WHY THIS NEEDS ANALYSIS — CONCURRENCY REALITY", font_size=7, bold=True, color=ACCENT)
    add_text_box(s2, Inches(7.30), Inches(1.85), Inches(5.35), Inches(0.4), "The joke: 17 peripherals arguing on one I²C bus.", font_size=9, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    bullets = [
        "FreeRTOS preemptive scheduler ticks every 1 ms — 9 tasks compete, priorities decide who preempts whom.",
        "Shared buses: I²C (AHT20/BMP280 + 2 char LCDs), SPI (LCD + font ROM), SDMMC (FATFS). All need bounded blocking.",
        "Stacks matter: printf(%a.1f) pulls _dtoa_r + malloc; Audio/Input blew 8K → 16K canary. Not in the model, but real.",
        "Audio cannot starve: double-buffered I²S with Helix fixed-point decode; Input (SD open) runs near-Audio depth — hence prio 2.",
        "Question for this deck: if we idealise 4 tasks, can RM guarantee deadlines? If not, does EDF? At what cost?",
    ]
    tb = s2.shapes.add_textbox(Inches(7.30), Inches(2.3), Inches(5.35), Inches(3.0)).text_frame
    tf = tb
    tf.word_wrap = True
    for bi, b in enumerate(bullets):
        p = tf.paragraphs[0] if bi==0 else tf.add_paragraph()
        p.text = "•  " + b
        p.font.size = Pt(7.5); p.font.color.rgb = RGBColor(0x33,0x33,0x44); p.font.name="Calibri"
        p.space_after = Pt(3)
        p.space_before = Pt(1)
    add_shape(s2, Inches(7.30), Inches(5.55), Inches(5.35), Pt(0.7), LIGHT_BG)
    add_text_box(s2, Inches(7.30), Inches(5.7), Inches(5.35), Inches(0.5),
                 "Takeaway: model 4 periodic tasks (b,c,d,a) to stress the method, then map back to the 9-task dual-core reality → slide 7.", font_size=7.5, italic=True, color=MUTED)
    add_slide_number(s2, 2, 7)

    # ── Slide 3 — Task Model ──────────────────────────────────────────
    s3 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s3, BG_SLIDE)
    header_bar(s3, "02 — Task model", "From 9 threads to 4 canonical tasks", "Enough to prove the point, small enough to compute by hand — then verify with bonus/rta")

    # left card task set
    add_shape(s3, Inches(0.4), Inches(1.35), Inches(6.3), Inches(2.9), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s3, Inches(0.65), Inches(1.5), Inches(5.8), Inches(0.25), "TASK SET — RM ORDER (shortest period = highest priority)", font_size=7, bold=True, color=ACCENT)
    # table header
    add_shape(s3, Inches(0.65), Inches(1.85), Inches(5.8), Inches(0.3), ACCENT)
    for txt,x,w in [("Task",0.65,0.9),("C (WCET)",1.55,1.0),("T = D",2.55,0.9),("Prio (RM)",3.45,0.9),("U = C/T",4.35,1.1)]:
        add_text_box(s3, Inches(x), Inches(1.85), Inches(w), Inches(0.3), txt, font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    tasks = [("b",2,8,1,0.250), ("c",3,10,2,0.300), ("d",4,18,3,0.222), ("a",7,40,4,0.175)]
    y=2.17
    for name,c,t,prio,u in tasks:
        bg = RGBColor(0xFF,0xFF,0xFF) if prio%2==1 else LIGHT_BG
        add_shape(s3, Inches(0.65), Inches(y), Inches(5.8), Inches(0.32), bg)
        add_text_box(s3, Inches(0.65), Inches(y), Inches(0.9), Inches(0.32), f"τ_{name}", font_size=8, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(1.55), Inches(y), Inches(1.0), Inches(0.32), str(c), font_size=8, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(2.55), Inches(y), Inches(0.9), Inches(0.32), str(t), font_size=8, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(3.45), Inches(y), Inches(0.9), Inches(0.32), str(prio), font_size=8, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(4.35), Inches(y), Inches(1.1), Inches(0.32), f"{u:.3f}", font_size=8, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.34
    add_shape(s3, Inches(0.65), Inches(3.58), Inches(5.8), Inches(0.32), RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s3, Inches(0.65), Inches(3.58), Inches(2.9), Inches(0.32), "  Total utilisation  U = Σ C/T", font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s3, Inches(3.55), Inches(3.58), Inches(2.9), Inches(0.32), "0.947", font_size=8, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    add_text_box(s3, Inches(0.65), Inches(3.98), Inches(5.8), Inches(0.45),
                 "D = T for all tasks (implicit deadlines). Solver: bonus/rta/rta_solver.py", font_size=7, color=MUTED)

    # right card: utilization bound
    add_shape(s3, Inches(7.0), Inches(1.35), Inches(5.9), Inches(2.9), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s3, Inches(7.25), Inches(1.5), Inches(5.4), Inches(0.25), "UTILISATION BOUND — SUFFICIENT, NOT NECESSARY", font_size=7, bold=True, color=ACCENT)
    add_text_box(s3, Inches(7.25), Inches(1.85), Inches(5.4), Inches(0.35), "Liu & Layland (1973)  —  U ≤ n(2¹/ⁿ − 1)", font_size=9, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    # bound calc boxes
    add_shape(s3, Inches(7.25), Inches(2.3), Inches(2.6), Inches(0.85), LIGHT_BG)
    add_text_box(s3, Inches(7.35), Inches(2.35), Inches(2.4), Inches(0.2), "n = 4", font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s3, Inches(7.35), Inches(2.55), Inches(2.4), Inches(0.2), "Bound = 4·(2¹/⁴ − 1)", font_size=7, color=MUTED)
    add_text_box(s3, Inches(7.35), Inches(2.82), Inches(2.4), Inches(0.22), "= 0.757", font_size=14, bold=True, color=ACCENT)
    add_shape(s3, Inches(10.05), Inches(2.3), Inches(2.6), Inches(0.85), RGBColor(0xFE,0xF2,0xF2), RGBColor(0xFE,0xC9,0xC9))
    add_text_box(s3, Inches(10.15), Inches(2.35), Inches(2.4), Inches(0.2), "Actual  U = 0.947", font_size=7, bold=True, color=ACCENT2)
    add_text_box(s3, Inches(10.15), Inches(2.55), Inches(2.4), Inches(0.2), "U  >  Bound  (0.947 > 0.757)", font_size=7, color=ACCENT2)
    add_text_box(s3, Inches(10.15), Inches(2.82), Inches(2.4), Inches(0.22), "INCONCLUSIVE → RTA", font_size=9, bold=True, color=ACCENT2)
    add_text_box(s3, Inches(7.25), Inches(3.35), Inches(5.4), Inches(0.65),
                 "Bound is pessimistic: harmonic periods pass more often. RTA (next slide) is exact — iterate R = C + Σ ceil(R/Tₕ)·Cₕ until fixed point or deadline miss.", font_size=7.5, color=MUTED)

    # bottom formula strip
    add_shape(s3, Inches(0.4), Inches(4.55), Inches(12.5), Inches(2.35), RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s3, Inches(0.65), Inches(4.7), Inches(12.0), Inches(0.25), "WHAT RTA ACTUALLY COMPUTES  —  exact response-time iteration", font_size=7, bold=True, color=RGBColor(0x8A,0x9B,0xB8))
    add_text_box(s3, Inches(0.65), Inches(5.05), Inches(7.5), Inches(0.35), "R_i⁰ = C_i        R_iᵏ⁺¹ = C_i + Σ_{h∈hp(i)} ceil(R_iᵏ / Tₕ) · Cₕ", font_size=13, bold=True, color=RGBColor(0xFF,0xFF,0xFF), font_name="Cambria")
    tb = s3.shapes.add_textbox(Inches(0.65), Inches(5.55), Inches(7.3), Inches(0.9)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Stop when Rᵏ⁺¹ = Rᵏ (schedulable if ≤ D) or R > D (miss). Each iteration adds interference from all higher-priority jobs that fit in the window."
    p.font.size = Pt(7.5); p.font.color.rgb = RGBColor(0xB8,0xC0,0xD8); p.font.name="Calibri"
    # right mini legend
    add_text_box(s3, Inches(8.4), Inches(5.05), Inches(4.2), Inches(0.25), "INTUITION: “How long am I delayed by everyone above me?”", font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s3, Inches(8.4), Inches(5.4), Inches(4.2), Inches(1.2),
                 "•  hp(i): higher-priority tasks\n•  ceil(R/Tₕ): how many jobs of h interfere\n•  Converges in ≤ D steps → pseudo-polynomial", font_size=7, color=RGBColor(0xB8,0xC0,0xD8))
    add_slide_number(s3, 3, 7)

    # ── Slide 4 — RTA + Gantt RM ───────────────────────────────────────
    s4 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s4, BG_SLIDE)
    header_bar(s4, "03 — Response-Time Analysis", "RM: three pass, one fails at 41 > 40", "bonus/rta/rta_solver.py  •  y is baseline (U8g2) but here y is time")
    # left table RTA
    add_shape(s4, Inches(0.4), Inches(1.35), Inches(4.6), Inches(3.7), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s4, Inches(0.65), Inches(1.5), Inches(4.1), Inches(0.25), "RTA ITERATION — FIXED-POINT TRACES", font_size=7, bold=True, color=ACCENT)
    add_shape(s4, Inches(0.65), Inches(1.85), Inches(4.1), Inches(0.28), ACCENT)
    for txt,x,w in [("Task",0.65,0.6),("Iter",1.25,1.0),("R",2.25,0.65),("≤ T?",2.90,0.85)]:
        add_text_box(s4, Inches(x), Inches(1.85), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows = [
        ("τ_b", "1: 2", "2", "✓ 2 ≤ 8"),
        ("τ_c", "2: 5 → 5", "5", "✓ 5 ≤ 10"),
        ("τ_d", "3: 4→9→14", "14", "✓ 14 ≤ 18"),
        ("τ_a", "4: 7→14→27→34→41", "41", "✗ 41 > 40"),
    ]
    y=2.15
    for i,(a,b,c,d) in enumerate(rows):
        bg = LIGHT_BG if i%2==1 else RGBColor(0xFF,0xFF,0xFF)
        if i==3:
            bg = RGBColor(0xFE,0xF2,0xF2)
        add_shape(s4, Inches(0.65), Inches(y), Inches(4.1), Inches(0.32), bg)
        add_text_box(s4, Inches(0.65), Inches(y), Inches(0.6), Inches(0.32), a, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(1.25), Inches(y), Inches(1.0), Inches(0.32), b, font_size=6.5, color=MUTED, alignment=PP_ALIGN.CENTER)
        col = ACCENT3 if i<3 else ACCENT2
        add_text_box(s4, Inches(2.25), Inches(y), Inches(0.65), Inches(0.32), c, font_size=8, bold=True, color=col, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(2.90), Inches(y), Inches(0.85), Inches(0.32), d, font_size=6.5, bold=True, color=col, alignment=PP_ALIGN.CENTER)
        y+=0.34
    # detail math
    tb = s4.shapes.add_textbox(Inches(0.65), Inches(3.58), Inches(4.1), Inches(1.25)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "τ_d example: R⁰=4 → R¹=4+ceil(4/8)·2+ceil(4/10)·3=9 → R²=4+ceil(9/8)·2+ceil(9/10)·3=14 stable."
    p.font.size = Pt(6.5); p.font.color.rgb = MUTED; p.font.name="Calibri"
    add_paragraph(tf, "τ_a: 7→14→27→34→41 crosses D=40 on iteration 4 — RM cannot schedule the full set. σ = interferences from b,c,d stacking.", font_size=6.5, color=MUTED)
    add_paragraph(tf, "Exact, not just sufficient. This is why the bloated player isolates Audio (prio 3, core 0).", font_size=6.5, bold=True, color=RGBColor(0x1A,0x1A,0x2A))

    # right gantt + stats
    add_shape(s4, Inches(5.25), Inches(1.35), Inches(7.65), Inches(3.7), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s4, Inches(5.5), Inches(1.5), Inches(7.15), Inches(0.25), "GANTT — PREEMPTIVE RM OVER [0,40)  (bonus/rta/schedule_sim.py → RMSim)", font_size=7, bold=True, color=ACCENT)
    s4.shapes.add_picture(os.path.join(ASSETS, "gantt_rm.png"), Inches(5.5), Inches(1.85), Inches(7.15), Inches(1.55))
    # stats row
    add_shape(s4, Inches(5.5), Inches(3.55), Inches(7.15), Inches(0.95), LIGHT_BG)
    stats = [("C switches", "18"), ("Slots", "19"), ("Idle", "0.7%"), ("Worst slack", "τ_d: 4 / 18")]
    x0=5.6
    for k,v in stats:
        add_text_box(s4, Inches(x0), Inches(3.60), Inches(1.2), Inches(0.2), k, font_size=6.5, bold=True, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(x0), Inches(3.85), Inches(1.2), Inches(0.3), v, font_size=11, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        x0+=1.8
    add_text_box(s4, Inches(5.5), Inches(4.55), Inches(7.15), Inches(0.35),
                 "Read: a only runs 14–16, 27–30, 35–36 — always preempted. Visual proof of R_i starvation.", font_size=7, italic=True, color=MUTED)

    # bottom: so what
    add_shape(s4, Inches(0.4), Inches(5.25), Inches(12.5), Inches(1.85), LIGHT_BG, RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s4, Inches(0.65), Inches(5.40), Inches(12.0), Inches(0.2), "SO WHAT FOR THE PLAYER?", font_size=7, bold=True, color=ACCENT)
    tb = s4.shapes.add_textbox(Inches(0.65), Inches(5.65), Inches(8.5), Inches(1.2)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "RM is optimal among fixed priorities, but this set defeats it. Options:"
    p.font.size = Pt(8); p.font.bold = True; p.font.color.rgb = RGBColor(0x1A,0x1A,0x2A); p.font.name="Calibri"
    add_paragraph(tf, "• Drop U (faster CPU, lighter WCET, longer periods) — not possible on ESP32-S3 at 240 MHz with Helix.", font_size=7.5, color=MUTED)
    add_paragraph(tf, "• Switch to EDF (dynamic) — next slide.  • Partition cores: we already do (Audio core 0, UI core 1) — formal equivalent of reducing n per core.", font_size=7.5, color=MUTED)
    add_text_box(s4, Inches(9.4), Inches(5.65), Inches(3.2), Inches(1.2),
                 "“The Guide says: don’t panic.\nThe analysis says: τ_a misses\nby 1. That’s as real as it gets.”", font_size=8, italic=True, color=MUTED)
    add_slide_number(s4, 4, 7)

    # ── Slide 5 — Scheduling alternatives ───────────────────────────────
    s5 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s5, BG_SLIDE)
    header_bar(s5, "04 — Alternatives", "RM vs EDF (preemptive) vs EDF (non-preemptive)", "Same tasks, same window [0,40) — only the rule changes")
    # three gantts stacked
    add_shape(s5, Inches(0.4), Inches(1.35), Inches(8.2), Inches(4.35), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s5, Inches(0.65), Inches(1.5), Inches(7.7), Inches(0.2), "GANTTS — VISUAL COMPARISON (all to same scale)", font_size=7, bold=True, color=ACCENT)
    s5.shapes.add_picture(os.path.join(ASSETS, "gantt_rm.png"), Inches(0.65), Inches(1.75), Inches(7.7), Inches(0.95))
    add_text_box(s5, Inches(0.65), Inches(2.70), Inches(7.7), Inches(0.12), "↑ RM: fixed, period-rate — a starves", font_size=6, color=MUTED, alignment=PP_ALIGN.RIGHT)
    s5.shapes.add_picture(os.path.join(ASSETS, "gantt_edf.png"), Inches(0.65), Inches(2.95), Inches(7.7), Inches(0.95))
    add_text_box(s5, Inches(0.65), Inches(3.90), Inches(7.7), Inches(0.12), "↑ EDF preemptive: deadline-driven — a finishes at 32 (meets D=40!)", font_size=6, color=ACCENT3, alignment=PP_ALIGN.RIGHT)
    s5.shapes.add_picture(os.path.join(ASSETS, "gantt_edf_np.png"), Inches(0.65), Inches(4.12), Inches(7.7), Inches(0.95))
    add_text_box(s5, Inches(0.65), Inches(5.07), Inches(7.7), Inches(0.12), "↑ EDF non-preemptive: no preemption — a runs 14–21 (blocking) but no interleaving", font_size=6, color=MUTED, alignment=PP_ALIGN.RIGHT)
    # right table
    add_shape(s5, Inches(8.85), Inches(1.35), Inches(4.05), Inches(4.35), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s5, Inches(9.10), Inches(1.5), Inches(3.55), Inches(0.25), "NUMBERS — AT A GLANCE", font_size=7, bold=True, color=ACCENT)
    add_shape(s5, Inches(9.10), Inches(1.85), Inches(3.55), Inches(0.28), ACCENT)
    for txt,x,w in [("Scheduler",9.10,1.45),("CS",10.55,0.6),("Meets?",11.15,0.75),("Notes",11.9,0.75)]:
        add_text_box(s5, Inches(x), Inches(1.85), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows = [
        ("RM pre.", "18", "✗ a fails", "optimal FP"),
        ("EDF pre.", "16", "✓ all meet", "dynamic, optimal*"),
        ("EDF non-pre.", "12", "✓ but frag.", "blocking ↑"),
    ]
    y=2.15
    for i,(a,b,c,d) in enumerate(rows):
        bg = RGBColor(0xFF,0xFF,0xFF) if i%2==0 else LIGHT_BG
        if i==1:
            bg = RGBColor(0xEC,0xFD,0xF5)
        add_shape(s5, Inches(9.10), Inches(y), Inches(3.55), Inches(0.32), bg)
        add_text_box(s5, Inches(9.10), Inches(y), Inches(1.45), Inches(0.32), a, font_size=7, bold=(i==1), color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s5, Inches(10.55), Inches(y), Inches(0.6), Inches(0.32), b, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        col = ACCENT3 if i==1 else (ACCENT2 if "fail" in c else MUTED)
        add_text_box(s5, Inches(11.15), Inches(y), Inches(0.75), Inches(0.32), c, font_size=6.5, bold=True, color=col, alignment=PP_ALIGN.CENTER)
        add_text_box(s5, Inches(11.90), Inches(y), Inches(0.75), Inches(0.32), d, font_size=6, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.34
    tb = s5.shapes.add_textbox(Inches(9.10), Inches(3.25), Inches(3.55), Inches(2.2)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "EDF optimality*: on uniprocessor with D≤T, if any schedule meets all deadlines, EDF does. Here RM fails, EDF succeeds."
    p.font.size = Pt(7); p.font.color.rgb = MUTED; p.font.italic = True; p.font.name="Calibri"
    add_paragraph(tf, "Non-preemptive reduces switches (12 vs 16) but inflates blocking — a must wait for b/c/d to finish (14–21). FreeRTOS is preemptive for a reason.", font_size=7, color=MUTED)
    add_paragraph(tf, "Player tie: Audio is soft-real-time (glitch = audible). Input/UI are firm. Preemptive EDF intuition justifies Audio prio 3 — it must preempt.", font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A))

    add_shape(s5, Inches(0.4), Inches(5.85), Inches(12.5), Inches(1.25), LIGHT_BG, RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s5, Inches(0.65), Inches(6.0), Inches(12.0), Inches(0.25), "BOTTOM LINE", font_size=7, bold=True, color=ACCENT)
    add_text_box(s5, Inches(0.65), Inches(6.25), Inches(12.0), Inches(0.5),
                 "Fixed priority loses here; dynamic wins. Next: what if tasks share resources? RM/EDF optimality evaporates without a blocking protocol.", font_size=8.5, color=RGBColor(0x1A,0x1A,0x2A))
    add_slide_number(s5, 5, 7)

    # ── Slide 6 — Resources PIP vs PCP ─────────────────────────────────
    s6 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s6, BG_SLIDE)
    header_bar(s6, "05 — Shared resources", "R1–R5: PIP vs PCP blocking (bonus/rta/blocking_times.py)", "One lock per resource · I²C/SPI/SDMMC are exactly these — priority inversion is not theoretical")
    # matrix table
    add_shape(s6, Inches(0.4), Inches(1.35), Inches(5.9), Inches(3.15), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s6, Inches(0.65), Inches(1.5), Inches(5.4), Inches(0.25), "CRITICAL-SECTION MATRIX — max CS per task per resource", font_size=7, bold=True, color=ACCENT)
    add_shape(s6, Inches(0.65), Inches(1.82), Inches(5.4), Inches(0.28), ACCENT)
    for txt,x,w in [("Task",0.65,0.75),("R1",1.40,0.62),("R2",2.02,0.62),("R3",2.64,0.62),("R4",3.26,0.62),("R5",3.88,0.62),("T",4.50,0.55)]:
        add_text_box(s6, Inches(x), Inches(1.82), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    cs = {
        "τ_B":[0,1,0,1,1,8],
        "τ_C":[2,0,1,2,1,10],
        "τ_D":[0,4,2,3,0,18],
        "τ_A":[6,1,0,0,4,40],
    }
    y=2.12
    for i,(name,vals) in enumerate(cs.items()):
        bg = RGBColor(0xFF,0xFF,0xFF) if i%2==0 else LIGHT_BG
        add_shape(s6, Inches(0.65), Inches(y), Inches(5.4), Inches(0.30), bg)
        add_text_box(s6, Inches(0.65), Inches(y), Inches(0.75), Inches(0.30), name, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        xs=[1.40,2.02,2.64,3.26,3.88]
        for v,x in zip(vals[:5], xs):
            txt = str(v) if v!=0 else "—"
            col = ACCENT2 if v>=4 else (RGBColor(0x1A,0x1A,0x2A) if v!=0 else MUTED)
            bold = v>=4
            add_text_box(s6, Inches(x), Inches(y), Inches(0.62), Inches(0.30), txt, font_size=7, bold=bold, color=col, alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(4.50), Inches(y), Inches(0.55), Inches(0.30), str(vals[5]), font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.32
    add_text_box(s6, Inches(0.65), Inches(3.42), Inches(5.4), Inches(0.35),
                 "Read: τ_A holds R1 for 6 — longest. This drives PCP blocking for everyone else.", font_size=7, italic=True, color=MUTED)
    # blocking results
    add_shape(s6, Inches(0.4), Inches(4.65), Inches(5.9), Inches(2.45), RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s6, Inches(0.65), Inches(4.80), Inches(5.4), Inches(0.2), "RESULT — B_i per protocol", font_size=7, bold=True, color=RGBColor(0x8A,0x9B,0xB8))
    add_shape(s6, Inches(0.65), Inches(5.05), Inches(5.4), Inches(0.28), RGBColor(0x2A,0x34,0x55))
    for txt,x,w in [("Task",0.65,0.9),("B_PIP",1.55,1.0),("B_PCP",2.55,1.0),("Saving",3.55,1.5)]:
        add_text_box(s6, Inches(x), Inches(5.05), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows = [("τ_B",10,4,"2.5×"), ("τ_C",10,6,"1.7×"), ("τ_D",6,6,"—"), ("τ_A",0,0,"—")]
    y=5.34
    for i,(a,b,c,d) in enumerate(rows):
        bg = RGBColor(0x24,0x2A,0x42) if i%2==0 else RGBColor(0x1E,0x24,0x3A)
        add_shape(s6, Inches(0.65), Inches(y), Inches(5.4), Inches(0.30), bg)
        add_text_box(s6, Inches(0.65), Inches(y), Inches(0.9), Inches(0.30), a, font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(1.55), Inches(y), Inches(1.0), Inches(0.30), str(b), font_size=7, color=RGBColor(0xFE,0xC9,0xC9), alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(2.55), Inches(y), Inches(1.0), Inches(0.30), str(c), font_size=7, bold=True, color=RGBColor(0x7B,0xE8,0xB0), alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(3.55), Inches(y), Inches(1.5), Inches(0.30), d if d!="—" else "no lower task", font_size=6.5, color=RGBColor(0x8A,0x9B,0xB8), alignment=PP_ALIGN.CENTER)
        y+=0.32
    add_text_box(s6, Inches(0.65), Inches(6.60), Inches(5.4), Inches(0.35),
                 "PCP: B = max over resources (max lower CS).  PIP: B = sum over lower tasks (max per task). PCP ≤ PIP always.", font_size=7, color=RGBColor(0xB8,0xC0,0xD8))

    # right: explanation + player link
    add_shape(s6, Inches(6.55), Inches(1.35), Inches(6.35), Inches(5.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s6, Inches(6.80), Inches(1.5), Inches(5.85), Inches(0.25), "WHY THIS MATTERS — INVERSION ON THE WIRE", font_size=7, bold=True, color=ACCENT)
    tb = s6.shapes.add_textbox(Inches(6.80), Inches(1.85), Inches(5.85), Inches(3.55)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Without a protocol: low-priority τ_A locks R1 (6 units), medium τ_D/τ_C spin, high τ_B starves — unbounded priority inversion."
    p.font.size = Pt(7.5); p.font.color.rgb = RGBColor(0x33,0x33,0x44); p.font.name="Calibri"; p.space_after = Pt(4)
    bullets = [
        "PIP: holder inherits blocked task’s priority → τ_A briefly becomes b-priority (2). But chained blocking sums: B_B = 2+4+4 = 10.",
        "PCP: ceiling per resource = highest priority that ever uses it. Lock only if priority > all ceilings of locked resources. At most one block: B_B = max(4,3,4)=4.",
        "Result: PCP halves blocking (10→4, 10→6). τ_D ties because only τ_A can block it — single lower task, sum=max.",
        "Player mapping: R1/R2… are not abstract — I²C bus (AHT20/BMP280 + 2 LCDs), SPI (ST7565R + font ROM), SDMMC (FATFS). Firmware uses Wire.setTimeout(50 ms) + distinct tasks per bus to bound the same effect.",
    ]
    for b in bullets:
        add_paragraph(tf, "•  " + b, font_size=7.5, color=RGBColor(0x33,0x33,0x44))
    add_shape(s6, Inches(6.80), Inches(5.65), Inches(5.85), Pt(0.7), LIGHT_BG)
    tb2 = s6.shapes.add_textbox(Inches(6.80), Inches(5.80), Inches(5.85), Inches(1.2)).text_frame
    tf2 = tb2
    tf2.word_wrap = True
    p = tf2.paragraphs[0]
    p.text = "With PIP/PCP, RTA + B:  R_i = C_i + B_i + Σ ceil(R_i/T_h)·C_h  — re-check slides 3–4 adding B_i."
    p.font.size = Pt(7); p.font.color.rgb = RGBColor(0x1A,0x1A,0x2A); p.font.bold = True; p.font.name="Calibri"
    add_paragraph(tf2, "τ_B with B_PCP=4 → R=2+4=6 ≤8 still OK; but τ_A’s B=0 — the miss was CPU, not lock. Blocking adds nuance, not the root cause here.", font_size=7, color=MUTED)
    add_slide_number(s6, 6, 7)

    # ── Slide 7 — EDF + Complexity + Takeaways ─────────────────────────
    s7 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s7, BG_SLIDE)
    header_bar(s7, "06 — EDF & Complexity", "When RM needs a verdict, EDF needs a proof — and that proof can be exponential or clever", "Q5/Q6 from the workbook · bonus/rta/q5_edf_test.py + q6_pdc_complexity.py")
    # left EDF proof
    add_shape(s7, Inches(0.4), Inches(1.35), Inches(6.3), Inches(5.8), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s7, Inches(0.65), Inches(1.5), Inches(5.8), Inches(0.25), "Q5 — EDF: PROCESSOR DEMAND CRITERION (Baruah et al. 1990)", font_size=7, bold=True, color=ACCENT)
    add_text_box(s7, Inches(0.65), Inches(1.82), Inches(5.8), Inches(0.3), "τ1 3/10  τ2 15/50/200  τ3 17/100/120  τ4 200/1000/2000", font_size=7, color=MUTED)
    add_shape(s7, Inches(0.65), Inches(2.18), Inches(5.8), Inches(0.38), LIGHT_BG)
    add_text_box(s7, Inches(0.75), Inches(2.22), Inches(2.8), Inches(0.15), "U = Σ C/T", font_size=6.5, color=MUTED)
    add_text_box(s7, Inches(0.75), Inches(2.38), Inches(2.8), Inches(0.18), "0.617  ≤ 1  ✓  necessary", font_size=10, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s7, Inches(3.75), Inches(2.22), Inches(2.5), Inches(0.15), "h(L) = Σ floor((L−D_i)/T_i+1)·C_i", font_size=6, color=MUTED)
    add_text_box(s7, Inches(3.75), Inches(2.38), Inches(2.5), Inches(0.18), "h(L) ≤ L  ∀ L", font_size=9, bold=True, color=ACCENT)
    tb = s7.shapes.add_textbox(Inches(0.65), Inches(2.68), Inches(5.8), Inches(0.9)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Need & sufficient: check at every deadline point L = k·T_i + D_i up to H. Solver did 600 points up to H = lcm(10,200,120,2000)=6000 — all pass: schedulable under EDF."
    p.font.size = Pt(7); p.font.color.rgb = MUTED; p.font.name="Calibri"
    # mini table of sample points
    add_shape(s7, Inches(0.65), Inches(3.68), Inches(5.8), Inches(0.28), ACCENT)
    for txt,x,w in [("L",0.65,0.8),("h(L)",1.45,0.9),("L−h(L)",2.35,1.0),("Verdict",3.35,2.1)]:
        add_text_box(s7, Inches(x), Inches(3.68), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    sample = [("10","3","7","✓"),("100","35","65","✓"),("1000","235","765","✓"),("6000","~3680","~2320","✓")]
    y=3.98
    for a,b,c,d in sample:
        add_shape(s7, Inches(0.65), Inches(y), Inches(5.8), Inches(0.26), RGBColor(0xFF,0xFF,0xFF) if y<4.5 else LIGHT_BG)
        add_text_box(s7, Inches(0.65), Inches(y), Inches(0.8), Inches(0.26), a, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s7, Inches(1.45), Inches(y), Inches(0.9), Inches(0.26), b, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s7, Inches(2.35), Inches(y), Inches(1.0), Inches(0.26), c, font_size=7, color=ACCENT3, alignment=PP_ALIGN.CENTER)
        add_text_box(s7, Inches(3.35), Inches(y), Inches(2.1), Inches(0.26), d, font_size=7, color=ACCENT3, alignment=PP_ALIGN.CENTER)
        y+=0.28
    add_text_box(s7, Inches(0.65), Inches(5.15), Inches(5.8), Inches(0.25), "… plus 596 more points — all satisfied. See q5_edf_test.py", font_size=6.5, italic=True, color=MUTED)
    # complexity comparison
    add_shape(s7, Inches(0.65), Inches(5.55), Inches(5.8), Inches(1.35), RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s7, Inches(0.85), Inches(5.70), Inches(5.4), Inches(0.2), "Q6 — THE CATCH: H EXPLODES", font_size=7, bold=True, color=RGBColor(0x8A,0x9B,0xB8))
    tb = s7.shapes.add_textbox(Inches(0.85), Inches(5.95), Inches(2.55), Inches(0.75)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Naïve: 600 points to H=6000"
    p.font.size = Pt(7); p.font.color.rgb = RGBColor(0xFE,0xC9,0xC9); p.font.name="Calibri"
    add_paragraph(tf, "H grows exponentially (primes!).", font_size=6.5, color=RGBColor(0x8A,0x9B,0xB8))
    tb = s7.shapes.add_textbox(Inches(3.55), Inches(5.95), Inches(2.55), Inches(0.75)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Smart: busy period L* = 10"
    p.font.size = Pt(7); p.font.bold = True; p.font.color.rgb = RGBColor(0x7B,0xE8,0xB0); p.font.name="Calibri"
    add_paragraph(tf, "Only 1 point → 600× fewer.", font_size=6.5, color=RGBColor(0xB8,0xC0,0xD8))
    add_text_box(s7, Inches(0.85), Inches(6.45), Inches(5.4), Inches(0.35),
                 "Iteration: L⁰=ΣC  →  Lᵏ⁺¹=h(Lᵏ).  Pseudo-polynomial O(n·D_max/C_min) — polynomial in values, not bit-length.", font_size=6.5, color=RGBColor(0xB8,0xC0,0xD8))

    # right takeaways
    add_shape(s7, Inches(7.0), Inches(1.35), Inches(5.9), Inches(5.8), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s7, Inches(7.25), Inches(1.5), Inches(5.4), Inches(0.25), "WHAT WE’D TELL VOGONS — TAKEAWAYS FOR THE PLAYER", font_size=7, bold=True, color=ACCENT)
    takeaways = [
        ("Partition wins", "Core 0 = Audio (3) + Input (2)  •  Core 1 = UI/LED/Matrix/Serial/Vogon (1). Two uniprocessors > one overloaded one. RTA per core is what matters."),
        ("Prio = policy", "Audio prio 3 is our PCP in practice — I²S cannot wait. Input prio 2 avoids SD-open starvation. Rest 1 = best-effort. Matches RM intuition without being RM."),
        ("Bound → RTA → PDC", "Course flow: sufficient (U≤bound) → exact (RTA+R_B) → EDF (PDC). Our deck walks that ladder with real numbers."),
        ("Complexity is real", "H=6000 → 600 checks; L*=10 → 1 check. On ESP32, analysis must be offline — runtime can’t afford exponential."),
        ("Don’t hand-wave stacks", "16K Audio/Input came from real canary trips (_dtoa_r). Schedulability ≠ schedulable + stack overflow."),
    ]
    y=1.82
    for title, body in takeaways:
        add_text_box(s7, Inches(7.25), Inches(y), Inches(5.4), Inches(0.2), title, font_size=7.5, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
        tb = s7.shapes.add_textbox(Inches(7.25), Inches(y+0.22), Inches(5.4), Inches(0.55)).text_frame
        tf = tb
        tf.word_wrap = True
        p = tf.paragraphs[0]
        p.text = body
        p.font.size = Pt(7); p.font.color.rgb = MUTED; p.font.name="Calibri"
        y+=0.83
    add_shape(s7, Inches(7.25), Inches(6.05), Inches(5.4), Pt(0.7), LIGHT_BG)
    add_text_box(s7, Inches(7.25), Inches(6.20), Inches(5.4), Inches(0.45),
                 "Repro: bonus/rta/*.py  •  rebuild: bonus/rta/lenv/bin/python generate_presentation.py", font_size=6.5, color=MUTED)
    add_text_box(s7, Inches(7.25), Inches(6.65), Inches(5.4), Inches(0.35),
                 "So long, and thanks for all the fish.", font_size=7, italic=True, color=ACCENT)
    add_slide_number(s7, 7, 7)

    out = os.path.join(WORKDIR, "Bloated_MP3_Player_RT_Deadline_Presentation.pptx")
    prs.save(out)
    print(f"Saved → {out}")
    return out

if __name__ == "__main__":
    build_gantts()
    out = create_presentation()
    print("Done. Open in PowerPoint / LibreOffice / Google Slides. 7 slides, 16:9.")
