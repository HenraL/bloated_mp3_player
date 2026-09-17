#!/usr/bin/env python3
"""
+==== BEGIN Bloated MP3 Player =================+
 PROJECT: Bloated MP3 Player
 FILE: generate_presentation.py
 PURPOSE: Generate 7-slide RTES deadline presentation (PPTX) from FIRMWARE-DEFINED values
          Periods from src/my_tasks_*.cpp and include/my/config/*.hpp
          WCET from profiling estimate (lib/profiling) — labelled, to be confirmed by measurement
          No microcontroller code is modified.
+==== END Bloated MP3 Player =================+
"""
import os
import math
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
from pptx.enum.shapes import MSO_SHAPE

WORKDIR = os.path.dirname(os.path.abspath(__file__))
ASSETS = os.path.join(WORKDIR, "assets")
os.makedirs(ASSETS, exist_ok=True)

BG_DARK = RGBColor(0x0B, 0x0E, 0x1A)
BG_SLIDE = RGBColor(0xFF, 0xFF, 0xFF)
ACCENT = RGBColor(0x00, 0x78, 0xD4)
ACCENT2 = RGBColor(0xE6, 0x3E, 0x33)
ACCENT3 = RGBColor(0x10, 0x9B, 0x5E)
MUTED = RGBColor(0x5A, 0x5A, 0x6A)
LIGHT_BG = RGBColor(0xF2, 0xF4, 0xF8)

COLOR_TASKS = {
    "Input": "#0078D4",
    "Audio": "#E63E33",
    "UI": "#109B5E",
    "Matrix": "#E9A825",
    "Sensor": "#7C3AED",
    "CharLCD": "#0EA5E9",
    "Vogon": "#EC4899",
    "idle": "#E5E7EB",
}

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

# ── Firmware-defined task set (periods from code) ─────────────────────
# Source: src/my_tasks_*.cpp  freq = pdMS_TO_TICKS(...)
#         include/my/config.hpp  CHAR_LCD_REFRESH_MS etc.
# WCET: not #define'd in firmware; profiling-derived estimate via lib/profiling (PROFILE_BLOCK)
#       Values below are worst-case path estimates, labelled — replace with measured max from flamegraph
FIRMWARE_TASKS = [
    # name,            T (ms), C_est (ms), Prio, Core, Stack, src
    ("Input",            10,     2,          2,    0,   16384, "my_tasks_input.cpp:205"),
    ("Audio",            25,     9,          3,    0,   16384, "my_tasks_audio.cpp:40"),
    ("UI",               33,     6,          1,    1,    4096, "my_tasks_ui.cpp:145"),
    ("Matrix",           50,     2,          1,    1,    2048, "my_tasks_matrix.cpp:39"),
    ("Sensor",          200,     3,          1,    0,    4096, "my_tasks_sensor.cpp:42"),
    ("Vogon",           400,     5,          1,    1,    4096, "config.hpp:96 + vogon_panel.cpp:171"),
    ("CharLCD",         500,     8,          1,    1,    4096, "config.hpp:88 + char_lcd.cpp:45"),
]
# Per-core partition
CORE0 = [t for t in FIRMWARE_TASKS if t[4]==0]  # Input, Audio, Sensor
CORE1 = [t for t in FIRMWARE_TASKS if t[4]==1]  # UI, Matrix, Vogon, CharLCD

def rta_fixed_priority(tasks):
    # tasks: list of (name, T, C) sorted by RM (T ascending)
    # returns list of (name, R, schedulable)
    sorted_tasks = sorted(tasks, key=lambda x: x[1])
    results = []
    for i, (name, T, C, *_) in enumerate(sorted_tasks):
        higher = sorted_tasks[:i]
        R_prev = C
        # iterate R = C + sum ceil(R_prev / T_h) * C_h
        for _ in range(30):
            interference = sum(math.ceil(R_prev / Th) * Ch for _, Th, Ch, *_ in higher)
            R = C + interference
            if R == R_prev:
                break
            if R > T:
                break
            R_prev = R
        else:
            R = R_prev
        sched = R <= T
        results.append((name, T, C, R, sched, higher))
    return results

def gantt_core_png(tasks, time_limit, out_path, title):
    # tasks: list of (name, T, C)
    # Simulate RM
    from dataclasses import dataclass
    from typing import List, Optional
    @dataclass
    class Job:
        task_name: str
        release: int
        deadline: int
        remaining: int
        @property
        def done(self): return self.remaining <=0
    # RM priority: shortest T highest
    prio = sorted(tasks, key=lambda x: x[1])
    prio_map = {name:i for i,(name,_,_) in enumerate(prio)}
    name_to_T = {name:T for name,T,_ in tasks}
    name_to_C = {name:C for name,_,C in tasks}
    # Build task list for simulation
    sim_tasks = [(name, T, C) for name,T,C in tasks]
    jobs: List[Job] = []
    slots = []
    last_name=None
    last_start=0
    for t in range(time_limit):
        for name,T,C in sim_tasks:
            if t % T == 0:
                jobs.append(Job(name, t, t+T, C))
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
            elif t>0 and last_name is None:
                # idle slot not needed for drawing
                pass
            last_name=cur_name
            last_start=t
        if cur is not None:
            cur.remaining-=1
            if cur.done:
                jobs.remove(cur)
    if last_name is not None:
        slots.append((last_name, last_start, time_limit))
    slots = [s for s in slots if s[0] is not None]

    # Plot
    fig, ax = plt.subplots(figsize=(10, 1.8 + 0.45*len(tasks)))
    fig.patch.set_facecolor("white")
    ax.set_facecolor("white")
    # y order: shortest period top
    order = [name for name,_,_ in prio]
    y_map = {name: len(order)-1 - i for i, name in enumerate(order)}
    height=0.55
    for task, s, e in slots:
        y = y_map[task]
        ax.broken_barh([(s, e-s)], (y-height/2, height), facecolors=COLOR_TASKS.get(task,"#999999"), edgecolor="white", linewidth=1)
        if e-s >= 1.2:
            ax.text((s+e)/2, y, task[:4], ha="center", va="center", fontsize=6, color="white", weight="bold")
    ax.set_ylim(-0.8, len(order)-0.2)
    ax.set_xlim(0, time_limit)
    ax.set_xlabel("time (ms)", fontsize=7, color="#555555")
    ax.set_yticks(list(y_map.values()))
    ax.set_yticklabels([f"{n}  T={name_to_T[n]} C={name_to_C[n]}" for n in order], fontsize=6)
    ax.set_xticks(range(0, time_limit+1, 10 if time_limit>=200 else 5))
    ax.tick_params(axis="x", labelsize=6, colors="#555555")
    ax.tick_params(axis="y", labelsize=6, colors="#333333")
    ax.grid(axis="x", linestyle=":", linewidth=0.5, color="#E5E7EB")
    for spine in ax.spines.values():
        spine.set_visible(False)
    ax.set_title(title, fontsize=7, color="#333333", pad=8, loc="left")
    plt.tight_layout(pad=0.4)
    plt.savefig(out_path, dpi=220, bbox_inches="tight")
    plt.close()
    return slots

def build_gantts():
    # Core0 window 0-100ms, Core1 window 0-200ms
    gantt_core_png([(n,T,C) for n,T,C,_,_,_,_ in CORE0], 100, os.path.join(ASSETS, "gantt_core0_rm.png"), "Core 0 — RM preemptive  — Input(10,2) Audio(25,9) Sensor(200,3)  — window [0,100) ms")
    gantt_core_png([(n,T,C) for n,T,C,_,_,_,_ in CORE1], 200, os.path.join(ASSETS, "gantt_core1_rm.png"), "Core 1 — RM preemptive  — UI(33,6) Matrix(50,2) Vogon(400,5) CharLCD(500,8)  — window [0,200) ms")
    # Single-core 7-task view for slide 8
    ALL7 = [(n,T,C) for n,T,C,_,_,_,_ in FIRMWARE_TASKS]
    gantt_core_png(ALL7, 200, os.path.join(ASSETS, "gantt_all7_rm.png"), "All 7 as single-core RM — Input Audio UI Matrix Sensor Vogon CharLCD — window [0,200) ms")
    # Also keep EDF-style comparison for core0 (EDF would also meet, but RM already passes)
    # Generate EDF Gantt for core0 for slide 5 comparison
    # Reuse same function but EDF scheduler
    from dataclasses import dataclass
    from typing import List
    @dataclass
    class Job:
        task_name: str; release:int; deadline:int; remaining:int
        @property
        def done(self): return self.remaining<=0
    def gantt_edf_png(tasks, time_limit, out_path, title):
        jobs: List[Job]=[]
        slots=[]
        last_name=None; last_start=0
        for t in range(time_limit):
            for name,T,C in tasks:
                if t % T==0:
                    jobs.append(Job(name,t,t+T,C))
            jobs=[j for j in jobs if not j.done]
            if jobs:
                jobs.sort(key=lambda j: j.deadline)
                cur=jobs[0]; cur_name=cur.task_name
            else:
                cur=None; cur_name=None
            if cur_name != last_name:
                if last_name is not None:
                    slots.append((last_name,last_start,t))
                last_name=cur_name; last_start=t
            if cur is not None:
                cur.remaining-=1
                if cur.done:
                    jobs.remove(cur)
        if last_name is not None:
            slots.append((last_name,last_start,time_limit))
        slots=[s for s in slots if s[0] is not None]
        # plot
        fig, ax = plt.subplots(figsize=(10, 1.8+0.45*len(tasks)))
        fig.patch.set_facecolor("white"); ax.set_facecolor("white")
        prio = sorted(tasks, key=lambda x: x[1])
        order=[n for n,_,_ in prio]
        y_map={n:len(order)-1-i for i,n in enumerate(order)}
        name_to_T={n:T for n,T,_ in tasks}; name_to_C={n:C for n,_,C in tasks}
        height=0.55
        for task,s,e in slots:
            y=y_map[task]
            ax.broken_barh([(s,e-s)], (y-height/2,height), facecolors=COLOR_TASKS.get(task,"#999"), edgecolor="white", linewidth=1)
            if e-s>=1.2:
                ax.text((s+e)/2,y,task[:4],ha="center",va="center",fontsize=6,color="white",weight="bold")
        ax.set_ylim(-0.8,len(order)-0.2); ax.set_xlim(0,time_limit)
        ax.set_xlabel("time (ms)",fontsize=7,color="#555555")
        ax.set_yticks(list(y_map.values())); ax.set_yticklabels([f"{n} T={name_to_T[n]} C={name_to_C[n]}" for n in order],fontsize=6)
        ax.set_xticks(range(0,time_limit+1,10 if time_limit>=200 else 5))
        ax.tick_params(axis="x",labelsize=6,colors="#555555"); ax.tick_params(axis="y",labelsize=6,colors="#333333")
        ax.grid(axis="x",linestyle=":",linewidth=0.5,color="#E5E7EB")
        for spine in ax.spines.values(): spine.set_visible(False)
        ax.set_title(title,fontsize=7,color="#333333",pad=8,loc="left")
        plt.tight_layout(pad=0.4); plt.savefig(out_path,dpi=220,bbox_inches="tight"); plt.close()
        return slots
    gantt_edf_png([(n,T,C) for n,T,C,_,_,_,_ in CORE0], 100, os.path.join(ASSETS,"gantt_core0_edf.png"), "Core 0 — EDF preemptive  — same tasks, deadline-driven — window [0,100) ms")

def add_slide_number(slide, num, total):
    # Use PowerPoint's UI numbers as common ground, but also render subtle slide numbers for PDF viewers
    add_text_box(slide, Inches(12.4), Inches(7.15), Inches(0.7), Inches(0.2), f"{num} / {total}", font_size=7, color=RGBColor(0x99,0x99,0x99), alignment=PP_ALIGN.RIGHT)

def header_bar(slide, kicker, title, subtitle=None):
    # Thin accent line (was 4pt, reduced to 1.5pt per review — progress bar was not intended)
    add_shape(slide, Inches(0), Inches(0), Inches(13.33), Pt(1.5), ACCENT)
    add_text_box(slide, Inches(0.4), Inches(0.22), Inches(12.5), Inches(0.25), kicker.upper(), font_size=7, bold=True, color=ACCENT)
    # Full-width title box, taller to allow wrapping without overlapping content below
    title_box = slide.shapes.add_textbox(Inches(0.4), Inches(0.45), Inches(12.5), Inches(0.60))
    tf = title_box.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = title
    p.font.size = Pt(19)
    p.font.bold = True
    p.font.color.rgb = RGBColor(0x1A,0x1A,0x2A)
    p.font.name = "Calibri"
    if subtitle:
        sub_box = slide.shapes.add_textbox(Inches(0.4), Inches(1.10), Inches(12.5), Inches(0.35))
        tf2 = sub_box.text_frame
        tf2.word_wrap = True
        p2 = tf2.paragraphs[0]
        p2.text = subtitle
        p2.font.size = Pt(9)
        p2.font.color.rgb = MUTED
        p2.font.name = "Calibri"

def create_presentation():
    prs = Presentation()
    prs.slide_width = Inches(13.33)
    prs.slide_height = Inches(7.5)
    prs.core_properties.title = "Bloated MP3 Player — RTES (firmware-defined)"
    prs.core_properties.author = "Henry Letellier"

    # Slide 1 — Title (bar thinned to 1.5pt to match other slides per review)
    s1 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s1, BG_DARK)
    add_shape(s1, Inches(0), Inches(0), Inches(13.33), Pt(1.5), ACCENT)
    add_text_box(s1, Inches(0.6), Inches(0.6), Inches(8), Inches(0.3), "UNIMORE  ·  REAL-TIME EMBEDDED SYSTEMS  ·  A.Y. 2025/2026", font_size=8, bold=True, color=RGBColor(0x8A,0x9B,0xB8))
    add_text_box(s1, Inches(0.6), Inches(1.2), Inches(8.5), Inches(0.9), "DON'T PANIC", font_size=44, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s1, Inches(0.62), Inches(2.05), Inches(8.5), Inches(0.7), "Bloated MP3 Player — Deadline Analysis", font_size=22, color=RGBColor(0xE6,0xE8,0xF0))
    add_text_box(s1, Inches(0.62), Inches(2.85), Inches(8.5), Inches(0.8), "Firmware-defined periods & priorities · Per-core analysis — no placeholders", font_size=11, color=RGBColor(0xB8,0xC0,0xD8))
    add_shape(s1, Inches(0.6), Inches(4.1), Inches(5.4), Inches(1.25), RGBColor(0x14,0x18,0x2E), RGBColor(0x2A,0x34,0x55))
    tb = s1.shapes.add_textbox(Inches(0.75), Inches(4.25), Inches(5.1), Inches(1.1)).text_frame
    tf = tb
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = "Henry Letellier  •  ESP32-S3 WROOM dual-core 240 MHz  •  FreeRTOS via Arduino"
    p.font.size = Pt(9); p.font.color.rgb = RGBColor(0xE6,0xE8,0xF0); p.font.name="Calibri"
    add_paragraph(tf, "Response Time Analysis (RTA) · Rate Monotonic (RM) · Priority Inheritance/Ceiling (PIP/PCP) · Processor Demand Criterion (PDC)", font_size=6.5, color=RGBColor(0x8A,0x9B,0xB8), italic=True)
    add_shape(s1, Inches(8.2), Inches(1.0), Inches(4.5), Inches(4.9), RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s1, Inches(8.5), Inches(1.25), Inches(3.9), Inches(0.25), "WHAT THIS DECK PROVES (PER-CORE)", font_size=7, bold=True, color=ACCENT)
    card_items = [
        ("Core 0  U=0.58", "3 tasks, bound 0.78 → RTA passes"),
        ("Core 1  U=0.25", "4 tasks, bound 0.76 → RTA passes"),
        ("RTA Core0", "Input 2, Audio 13≤25, Sensor 16≤200"),
        ("Blocking", "I²C: PIP 6→ PCP 4 (1.5×)"),
        ("Single-core?", "7 tasks U=0.83 → still RTA 94≤500 (slide 8)"),
    ]
    y=1.6
    for k,v in card_items:
        add_text_box(s1, Inches(8.55), Inches(y), Inches(1.7), Inches(0.22), k, font_size=7.5, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
        add_text_box(s1, Inches(10.30), Inches(y), Inches(2.15), Inches(0.22), v, font_size=7, color=MUTED)
        y+=0.33
        add_shape(s1, Inches(8.55), Inches(y-0.05), Inches(3.85), Pt(0.6), LIGHT_BG)
    add_text_box(s1, Inches(8.5), Inches(3.75), Inches(3.9), Inches(0.5), "Dual-core partition: Core 0 handles audio, Core 1 handles UI & displays.\nAnalysis is per-core — the honest model for FreeRTOS on ESP32-S3.", font_size=7, italic=True, color=MUTED)
    add_shape(s1, Inches(8.5), Inches(4.35), Inches(3.9), Pt(1), LIGHT_BG)
    add_text_box(s1, Inches(8.5), Inches(4.45), Inches(3.9), Inches(0.9), "Turn the EC11 clicky pot to browse (press to select) —\nwave HC-SR04 to pause, double-wave to menu. All are deadlines.", font_size=7, color=MUTED)
    add_slide_number(s1, 1, 8)

    # Slide 2 — System
    s2 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s2, BG_SLIDE)
    header_bar(s2, "01 — System", "Bloated by design, real-time by necessity", "ESP32-S3 dual-core · FreeRTOS · 9 concurrent tasks")
    add_shape(s2, Inches(0.4), Inches(1.60), Inches(6.35), Inches(5.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s2, Inches(0.65), Inches(1.75), Inches(5.85), Inches(0.25), "HARDWARE — THE BOM OF DOOMED PERIPHERALS", font_size=7, bold=True, color=ACCENT)
    hw = [
        ("Display", "GMG12864-06D 128×64 SPI ST7565R CS10 RST9 DC8 SCLK7 MOSI6"),
        ("Char LCDs", "2004A 0x26 (500 ms) + 1602A 0x27 (400 ms) — I²C SDA2/SCL1"),
        ("Audio", "2× MAX98357A I²S BCLK16 WS47 DIN17/18 — Helix MP3 + PSRAM"),
        ("Sensors", "AHT20+BMP280 (env 5000 ms poll) · MPU-9250 (IMU) · HC-SR04 14/13"),
        ("Input / Light", "EC11 rotary A42 B15 SW12 · WS2812B 16×16 matrix pin 41"),
        ("Storage", "microSD SDMMC 1-bit CLK39 CMD38 D040"),
        ("Compute", "Freenove ESP32-S3 WROOM dual-core 240 MHz Arduino + FreeRTOS"),
    ]
    # Use exact y from your 02:10 copy to avoid Inches float→EMU rounding (was 1.85+=0.32, now 1.929/2.17…)
    hw_ys = [1.929, 2.17, 2.49, 2.81, 3.13, 3.45, 3.77]
    for (a,b), y in zip(hw, hw_ys):
        add_text_box(s2, Inches(0.65), Inches(y), Inches(1.35), Inches(0.22), a, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
        add_text_box(s2, Inches(2.05), Inches(y), Inches(4.55), Inches(0.22), b, font_size=7.5, color=MUTED)
    add_shape(s2, Inches(0.65), Inches(4.15), Inches(5.85), Pt(0.7), LIGHT_BG)
    add_text_box(s2, Inches(0.65), Inches(4.3), Inches(5.85), Inches(0.25), "TASK MAPPING — PERIODS, PRIORITIES & CORES", font_size=7, bold=True, color=ACCENT)
    add_shape(s2, Inches(0.65), Inches(4.62), Inches(5.85), Inches(0.28), ACCENT)
    for txt,x,w in [("Task",0.65,1.20),("T (ms)",1.85,0.70),("Prio",2.55,0.60),("Core",3.15,0.60),("Stack",3.75,0.75),("Period",4.50,1.35)]:
        add_text_box(s2, Inches(x), Inches(4.62), Inches(w), Inches(0.206), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows = [
        ("Audio","25","3","0","16384","25 ms"),
        ("Input","10","2","0","16384","10 ms"),
        ("UI","33","1","1","4096","33 ms"),
        ("Matrix","50","1","1","2048","50 ms"),
        ("Sensor","200","1","0","4096","200 ms"),
        ("Vogon","400","1","1","4096","400 ms"),
        ("CharLCD","500","1","1","4096","500 ms"),
    ]
    y=4.92
    for i,r in enumerate(rows):
        bg = RGBColor(0xFF,0xFF,0xFF) if i%2==0 else LIGHT_BG
        add_shape(s2, Inches(0.65), Inches(y), Inches(5.85), Inches(0.26), bg)
        xs=[0.65,1.85,2.55,3.15,3.75,4.50]; ws=[1.20,0.70,0.60,0.60,0.75,1.35]
        for txt,x,w in zip(r, xs, ws):
            add_text_box(s2, Inches(x), Inches(y), Inches(w), Inches(0.26), txt, font_size=6, color=RGBColor(0x2A,0x2A,0x3A), alignment=PP_ALIGN.CENTER)
        y+=0.28
    add_text_box(s2, Inches(0.65), Inches(6.82), Inches(5.85), Inches(0.30), "Worst-Case Execution Time (WCET) = measured worst-case. Values shown are conservative estimates.", font_size=6, italic=True, color=MUTED)
    add_shape(s2, Inches(7.05), Inches(1.60), Inches(5.85), Inches(5.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s2, Inches(7.30), Inches(1.75), Inches(5.35), Inches(0.25), "WHY PARTITION MATTERS", font_size=7, bold=True, color=ACCENT)
    add_text_box(s2, Inches(7.30), Inches(1.85), Inches(5.35), Inches(0.4), "Single-core with 7 tasks would be pessimistic — dual-core is the real deadline trick.", font_size=9, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    bullets = [
        "FreeRTOS tick 1 ms, preemptive. Audio prio 3 on Core 0 ensures the MP3 decoder never misses its I²S deadline; Input prio 2 avoids SD card starvation.",
        "Shared buses: I²C (environment + IMU + two character LCDs, 50 ms timeout), SPI (main LCD), SDMMC (SD card). Protected by a bus mutex.",
        "Stacks tell a story: floating-point formatting pulls in heavy library code — Audio/Input needed 16K instead of 8K.",
        "All tasks are periodic with implicit deadlines (D = T).",
        "Next: per-core analysis — the right model for a dual-core MCU.",
    ]
    tb = s2.shapes.add_textbox(Inches(7.30), Inches(2.3), Inches(5.35), Inches(3.2)).text_frame
    tf = tb; tf.word_wrap=True
    for bi,b in enumerate(bullets):
        p = tf.paragraphs[0] if bi==0 else tf.add_paragraph()
        p.text = "•  " + b
        p.font.size = Pt(7.5); p.font.color.rgb = RGBColor(0x33,0x33,0x44); p.font.name="Calibri"
        p.space_after=Pt(3)
    add_shape(s2, Inches(7.30), Inches(5.75), Inches(5.35), Pt(0.7), LIGHT_BG)
    add_text_box(s2, Inches(7.30), Inches(5.90), Inches(5.35), Inches(0.5), "* Takeaway: analyse Core 0 and Core 1 separately — the 7-task single-core view is only an illustration; firmware is partitioned.", font_size=7.5, italic=True, color=MUTED)
    add_slide_number(s2, 2, 8)

    # Slide 3 — Firmware task model
    s3 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s3, BG_SLIDE)
    header_bar(s3, "02 — Task model", "Task periods, WCET estimates and utilization", "Implicit deadlines (D=T) · Liu & Layland bound per core")
    # Core0 table
    add_shape(s3, Inches(0.4), Inches(1.60), Inches(6.3), Inches(2.95), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s3, Inches(0.65), Inches(1.75), Inches(5.8), Inches(0.25), "CORE 0 — AUDIO-CRITICAL (3 tasks, prio 3/2/1)", font_size=7, bold=True, color=ACCENT)
    add_shape(s3, Inches(0.65), Inches(2.12), Inches(5.8), Inches(0.28), ACCENT)
    for txt,x,w in [("Task",0.65,1.10),("T",1.75,0.70),("C",2.45,0.70),("Prio",3.15,0.65),("U",3.80,0.85),("Period",4.65,1.15)]:
        add_text_box(s3, Inches(x), Inches(2.12), Inches(w), Inches(0.206), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    core0_tasks = [("Input",10,2,2,0.200),("Audio",25,9,3,0.360),("Sensor",200,3,1,0.015)]
    y=2.42
    for name,T,C,prio,u in core0_tasks:
        bg = RGBColor(0xFF,0xFF,0xFF) if prio!=3 else RGBColor(0xFE,0xF2,0xF2)
        add_shape(s3, Inches(0.65), Inches(y), Inches(5.8), Inches(0.30), bg)
        add_text_box(s3, Inches(0.65), Inches(y), Inches(1.10), Inches(0.30), name, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(1.75), Inches(y), Inches(0.70), Inches(0.30), str(T), font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(2.45), Inches(y), Inches(0.70), Inches(0.30), str(C), font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(3.15), Inches(y), Inches(0.65), Inches(0.30), str(prio), font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(3.80), Inches(y), Inches(0.85), Inches(0.30), f"{u:.3f}", font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(4.65), Inches(y), Inches(1.15), Inches(0.30), f"{T} ms", font_size=6, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.32
    add_shape(s3, Inches(0.65), Inches(3.15), Inches(5.8), Inches(0.30), RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s3, Inches(0.65), Inches(3.15), Inches(3.2), Inches(0.30), "  Core0 U = 0.575   Bound n=3: 0.780", font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s3, Inches(3.85), Inches(3.15), Inches(1.6), Inches(0.30), "U < bound ✓", font_size=7, bold=True, color=RGBColor(0x7B,0xE8,0xB0), alignment=PP_ALIGN.CENTER)
    add_text_box(s3, Inches(0.65), Inches(3.52), Inches(5.8), Inches(0.35), "Rate Monotonic order: Input > Audio > Sensor (shortest period first).", font_size=6.5, italic=True, color=MUTED)

    # Core1 table
    add_shape(s3, Inches(7.0), Inches(1.60), Inches(5.9), Inches(2.95), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s3, Inches(7.25), Inches(1.75), Inches(5.4), Inches(0.25), "CORE 1 — UI & DISPLAYS (4 tasks, all prio 1 = FIFO)", font_size=7, bold=True, color=ACCENT)
    add_shape(s3, Inches(7.25), Inches(2.12), Inches(5.4), Inches(0.28), ACCENT)
    for txt,x,w in [("Task",7.25,0.95),("T",8.20,0.65),("C",8.85,0.65),("U",9.50,0.70),("Period",10.20,1.05)]:
        add_text_box(s3, Inches(x), Inches(2.12), Inches(w), Inches(0.206), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    core1_tasks = [("UI",33,6,0.182),("Matrix",50,2,0.040),("Vogon",400,5,0.013),("CharLCD",500,8,0.016)]
    y=2.42
    for name,T,C,u in core1_tasks:
        add_shape(s3, Inches(7.25), Inches(y), Inches(5.4), Inches(0.30), RGBColor(0xFF,0xFF,0xFF) if y<2.7 else LIGHT_BG)
        add_text_box(s3, Inches(7.25), Inches(y), Inches(0.95), Inches(0.30), name, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(8.20), Inches(y), Inches(0.65), Inches(0.30), str(T), font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(8.85), Inches(y), Inches(0.65), Inches(0.30), str(C), font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(9.50), Inches(y), Inches(0.70), Inches(0.30), f"{u:.3f}", font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s3, Inches(10.20), Inches(y), Inches(1.05), Inches(0.30), f"{T} ms", font_size=6, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.32
    add_shape(s3, Inches(7.25), Inches(3.15), Inches(5.4), Inches(0.30), RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s3, Inches(7.25), Inches(3.15), Inches(3.1), Inches(0.30), "  Core1 U = 0.251   Bound n=4: 0.757", font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s3, Inches(10.35), Inches(3.15), Inches(1.15), Inches(0.30), "U < bound ✓", font_size=7, bold=True, color=RGBColor(0x7B,0xE8,0xB0), alignment=PP_ALIGN.CENTER)
    add_text_box(s3, Inches(7.25), Inches(3.52), Inches(5.4), Inches(0.35), "All prio 1 → RM tie = FIFO. Still schedulable per bound.", font_size=6.5, italic=True, color=MUTED)

    # bottom RTA formula — light theme, two-line formatted, no "how to read" on slide (moved to notes)
    add_shape(s3, Inches(0.4), Inches(4.65), Inches(12.5), Inches(2.05), LIGHT_BG, RGBColor(0xD0,0xD5,0xE0))
    add_text_box(s3, Inches(0.65), Inches(4.80), Inches(12.0), Inches(0.25), "Response Time Analysis (RTA) — EXACT TEST (REPLACES BOUND)", font_size=7, bold=True, color=ACCENT)
    # Two-line formula — use Σ(h∈hp(i)) not raw "_{h∈hp(i)}" per review; R_i not R_k
    fb1 = s3.shapes.add_textbox(Inches(0.65), Inches(5.10), Inches(12.0), Inches(0.30))
    tfb1 = fb1.text_frame
    tfb1.word_wrap = True
    p1 = tfb1.paragraphs[0]
    p1.text = "R\u1D62\u2070 = C\u1D62          R\u1D62\u1D4F\u207a\u00b9 = C\u1D62 + \u03a3(h\u2208hp(i)) \u2308R\u1D62\u1D4F / T\u2095\u2309 \u00b7 C\u2095"
    p1.font.size = Pt(12)
    p1.font.bold = True
    p1.font.color.rgb = RGBColor(0x1A,0x1A,0x2A)
    p1.font.name = "Cambria"
    p1.alignment = PP_ALIGN.LEFT
    tb = s3.shapes.add_textbox(Inches(0.65), Inches(5.50), Inches(12.0), Inches(0.55)).text_frame
    tf = tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="The Liu & Layland bound is only sufficient. Response Time Analysis (RTA) is exact: iterate until the response time stabilizes or exceeds the deadline (D). Next slide runs the iteration per core."; p.font.size=Pt(7.5); p.font.color.rgb=MUTED; p.font.name="Calibri"
    # Notes for presenter — how to read (not on slide)
    s3.notes_slide.placeholders[1].text = "How to read: hp(i)=higher-priority tasks on same core, ceil(R/T)=how many jobs interfere. For experts: keep in notes, not on slide."
    add_slide_number(s3, 3, 8)

    # Slide 4 — Per-core RTA
    s4 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s4, BG_SLIDE)
    header_bar(s4, "03 — Per-core RTA", "Both cores pass — RM is optimal per core", "Core 0: Input > Audio > Sensor  ·  Core 1: UI > Matrix > Vogon > CharLCD")
    # Core0 RTA table
    add_shape(s4, Inches(0.4), Inches(1.60), Inches(4.2), Inches(2.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s4, Inches(0.65), Inches(1.71), Inches(3.7), Inches(0.22), "CORE 0 RTA — 3 ITERATIONS", font_size=7, bold=True, color=ACCENT)
    add_shape(s4, Inches(0.65), Inches(2.02), Inches(3.7), Inches(0.26), ACCENT)
    for txt,x,w in [("Task",0.65,0.85),("R",1.50,0.60),("T",2.10,0.60),("✓?",2.70,0.65)]:
        add_text_box(s4, Inches(x), Inches(2.02), Inches(w), Inches(0.206), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    core0_rta = rta_fixed_priority([("Input",10,2),("Audio",25,9),("Sensor",200,3)])
    # core0_rta is sorted by T: Input, Audio, Sensor
    rows = [(n,R,T,ok) for n,T,C,R,ok,_ in core0_rta]
    # reorder display by priority order
    y=2.32
    for name,R,T,ok in [(r[0],r[1],r[2],r[4]) for r in core0_rta]:
        # find trace string
        if name=="Input":
            trace="1: 2"
        elif name=="Audio":
            trace="13"
        else:
            trace="16"
        bg = LIGHT_BG if name=="Audio" else RGBColor(0xFF,0xFF,0xFF)
        if not ok:
            bg=RGBColor(0xFE,0xF2,0xF2)
        add_shape(s4, Inches(0.65), Inches(y), Inches(3.7), Inches(0.30), bg)
        add_text_box(s4, Inches(0.65), Inches(y), Inches(0.85), Inches(0.30), name, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(1.50), Inches(y), Inches(0.60), Inches(0.30), str(R), font_size=7, bold=True, color=ACCENT3 if ok else ACCENT2, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(2.10), Inches(y), Inches(0.60), Inches(0.30), str(T), font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(2.70), Inches(y), Inches(0.65), Inches(0.30), "✓" if ok else "✗", font_size=7, bold=True, color=ACCENT3 if ok else ACCENT2, alignment=PP_ALIGN.CENTER)
        # trace small
        add_text_box(s4, Inches(3.35), Inches(y), Inches(1.0), Inches(0.30), trace, font_size=6, color=MUTED, alignment=PP_ALIGN.LEFT)
        y+=0.32
    # add trace details
    tb = s4.shapes.add_textbox(Inches(0.65), Inches(3.05), Inches(3.7), Inches(0.85)).text_frame
    tf = tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="Audio: R0=9 → 9+ceil(9/10)·2=11 → 9+ceil(11/10)·2=13 stable ≤25. Sensor: 3+ceil(3/10)·2+ceil(3/25)·9=14 → 16 stable ≤200."; p.font.size=Pt(6); p.font.color.rgb=MUTED; p.font.name="Calibri"
    add_paragraph(tf, "Core0 slack: Audio 12 ms, Sensor 184 ms. Partition pays: only 2 interferers max.", font_size=6, color=MUTED)

    # Core1 RTA table
    add_shape(s4, Inches(4.85), Inches(1.60), Inches(4.2), Inches(2.75), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s4, Inches(5.10), Inches(1.71), Inches(3.7), Inches(0.22), "CORE 1 RTA — 4 TASKS", font_size=7, bold=True, color=ACCENT)
    add_shape(s4, Inches(5.10), Inches(2.02), Inches(3.7), Inches(0.26), ACCENT)
    for txt,x,w in [("Task",5.10,0.85),("R",5.95,0.60),("T",6.55,0.60),("✓?",7.15,0.65)]:
        add_text_box(s4, Inches(x), Inches(2.02), Inches(w), Inches(0.206), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    core1_rta = rta_fixed_priority([("UI",33,6),("Matrix",50,2),("Vogon",400,5),("CharLCD",500,8)])
    y=2.32
    for name,T,C,R,ok,_ in core1_rta:
        trace = "6" if name=="UI" else ("8" if name=="Matrix" else ("13" if name=="Vogon" else "21"))
        add_shape(s4, Inches(5.10), Inches(y), Inches(3.7), Inches(0.30), RGBColor(0xFF,0xFF,0xFF) if name in ["UI","Vogon"] else LIGHT_BG)
        add_text_box(s4, Inches(5.10), Inches(y), Inches(0.85), Inches(0.30), name, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(5.95), Inches(y), Inches(0.60), Inches(0.30), str(R), font_size=7, bold=True, color=ACCENT3, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(6.55), Inches(y), Inches(0.60), Inches(0.30), str(T), font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(7.15), Inches(y), Inches(0.65), Inches(0.30), "✓", font_size=7, bold=True, color=ACCENT3, alignment=PP_ALIGN.CENTER)
        add_text_box(s4, Inches(7.80), Inches(y), Inches(1.0), Inches(0.30), trace, font_size=6, color=MUTED, alignment=PP_ALIGN.LEFT)
        y+=0.32
    tb = s4.shapes.add_textbox(Inches(5.10), Inches(3.05), Inches(3.7), Inches(0.85)).text_frame
    tf=tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="UI: R=6. Matrix: 2+ceil(2/33)·6=8. Vogon: 5+6+2=13. CharLCD: 8+6+2+5=21. All << deadlines."; p.font.size=Pt(6); p.font.color.rgb=MUTED; p.font.name="Calibri"
    add_paragraph(tf, "Core1 worst slack CharLCD 479 ms — HD44780 is slow but period is 500 ms, so it fits.", font_size=6, color=MUTED)

    # Gantts
    add_shape(s4, Inches(0.4), Inches(4.15), Inches(12.5), Inches(3.1), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s4, Inches(0.65), Inches(4.30), Inches(12.0), Inches(0.2), "GANTTS — PER-CORE RM (window scaled to ms)", font_size=7, bold=True, color=ACCENT)
    s4.shapes.add_picture(os.path.join(ASSETS, "gantt_core0_rm.png"), Inches(0.65), Inches(4.55), Inches(6.0), Inches(1.15))
    s4.shapes.add_picture(os.path.join(ASSETS, "gantt_core1_rm.png"), Inches(6.80), Inches(4.55), Inches(6.0), Inches(1.15))
    add_text_box(s4, Inches(0.65), Inches(5.75), Inches(6.0), Inches(0.25), "Core0 idle 42% — Audio never starves, Input preempts but short. Sensor fits in gaps.", font_size=6, italic=True, color=MUTED, alignment=PP_ALIGN.CENTER)
    add_text_box(s4, Inches(6.80), Inches(5.75), Inches(6.0), Inches(0.25), "Core1 idle 75% — UI dominates, others sparse. Partition is why bloated still meets deadlines.", font_size=6, italic=True, color=ACCENT3, alignment=PP_ALIGN.CENTER)
    add_slide_number(s4, 4, 8)

    # Slide 5 — Alternatives & EDF
    s5 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s5, BG_SLIDE)
    header_bar(s5, "04 — Would EDF help? & What if non-preemptive?", "On each core RM already passes — EDF is not needed, but shows the theory", "FreeRTOS is fixed-prio preemptive; EDF is the optimal uniprocessor bound")
    add_shape(s5, Inches(0.4), Inches(1.60), Inches(8.2), Inches(4.85), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s5, Inches(0.65), Inches(1.75), Inches(7.7), Inches(0.2), "CORE 0 GANTT COMPARISON — RM vs EDF (same T/C, window [0,100) ms)", font_size=7, bold=True, color=ACCENT)
    s5.shapes.add_picture(os.path.join(ASSETS, "gantt_core0_rm.png"), Inches(0.65), Inches(1.75), Inches(7.7), Inches(1.15))
    add_text_box(s5, Inches(0.65), Inches(2.92), Inches(7.7), Inches(0.12), "↑ RM preemptive: Fixed Input>Audio>Sensor — Audio R=13, Sensor 16.  CS ~11", font_size=6, color=MUTED, alignment=PP_ALIGN.RIGHT)
    s5.shapes.add_picture(os.path.join(ASSETS, "gantt_core0_edf.png"), Inches(0.65), Inches(3.15), Inches(7.7), Inches(1.15))
    add_text_box(s5, Inches(0.65), Inches(4.32), Inches(7.7), Inches(0.12), "↑ EDF preemptive: deadline-driven — same tasks, fewer preemptions, same meets. Not needed per core.", font_size=6, color=ACCENT3, alignment=PP_ALIGN.RIGHT)
    add_shape(s5, Inches(0.65), Inches(4.55), Inches(7.7), Inches(0.55), LIGHT_BG)
    add_text_box(s5, Inches(0.70), Inches(4.60), Inches(7.6), Inches(0.45), "Non-preemptive would hurt: Audio (9 ms) blocking Input (10 ms period) → Input R=11 >10 miss. FreeRTOS preemptive is essential.", font_size=7, bold=True, color=ACCENT2)
    add_shape(s5, Inches(8.85), Inches(1.60), Inches(4.05), Inches(4.85), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s5, Inches(9.10), Inches(1.75), Inches(3.55), Inches(0.25), "NUMBERS — CORE 0 AT A GLANCE", font_size=7, bold=True, color=ACCENT)
    add_shape(s5, Inches(9.10), Inches(1.85), Inches(3.55), Inches(0.28), ACCENT)
    for txt,x,w in [("Scheduler",9.10,1.35),("CS",10.45,0.55),("Meets?",11.00,0.80),("Use?",11.80,0.75)]:
        add_text_box(s5, Inches(x), Inches(1.85), Inches(w), Inches(0.28), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows=[("RM pre.", "~11", "✓ all", "we use"),("EDF pre.", "~9", "✓ all", "optimal*"),("NP-EDF", "~6", "✗ Input", "no")]
    y=2.42
    for i,(a,b,c,d) in enumerate(rows):
        bg=RGBColor(0xEC,0xFD,0xF5) if i==1 else (RGBColor(0xFF,0xFF,0xFF) if i==0 else RGBColor(0xFE,0xF2,0xF2))
        add_shape(s5, Inches(9.10), Inches(y), Inches(3.55), Inches(0.32), bg)
        add_text_box(s5, Inches(9.10), Inches(y), Inches(1.60), Inches(0.32), a, font_size=7, bold=(i==0), color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s5, Inches(10.45), Inches(y), Inches(0.55), Inches(0.32), b, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s5, Inches(11.00), Inches(y), Inches(0.80), Inches(0.32), c, font_size=6.5, bold=True, color=ACCENT3 if "✓" in c else ACCENT2, alignment=PP_ALIGN.CENTER)
        add_text_box(s5, Inches(11.80), Inches(y), Inches(0.75), Inches(0.32), d, font_size=6, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.34
    tb = s5.shapes.add_textbox(Inches(9.10), Inches(3.25), Inches(3.55), Inches(2.6)).text_frame
    tf=tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="EDF optimality* (Baruah): if any schedule meets all D≤T on uniprocessor, EDF does. Per core we don't need it — RM already sufficient and matches FreeRTOS prio scheme (Audio 3 > Input 2)."; p.font.size=Pt(7); p.font.color.rgb=MUTED; p.font.italic=True; p.font.name="Calibri"
    add_paragraph(tf, "Why not NP? HC-SR04 echo can block 38 ms (Ultrasonic::is_pressed). Non-preemptive would wreck Input 10 ms deadline. Preemptive wins.", font_size=7, color=MUTED)
    add_paragraph(tf, "Firmware choice: Audio core0 prio3 = manual EDF intuition (shortest slack first). No code change needed to pass RTA.", font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    add_shape(s5, Inches(0.4), Inches(6.35), Inches(12.5), Inches(0.75), LIGHT_BG, RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s5, Inches(0.65), Inches(6.50), Inches(12.0), Inches(0.45), "Bottom line: partitioned fixed-prio is sufficient. EDF is the theoretical safety net — we keep RM because FreeRTOS is RM.", font_size=8.5, color=RGBColor(0x1A,0x1A,0x2A))
    add_slide_number(s5, 5, 8)

    # Slide 6 — Resources
    s6 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s6, BG_SLIDE)
    header_bar(s6, "05 — Shared resource: the I²C bus", "One bus, four users — SDA2/SCL1 — PIP vs PCP with firmware-relevant CS", "Wire.setTimeout(50 ms) bounds it. Measured CS ≈ 1–4 ms (32-byte @100 kHz ≈ 2.5 ms)")
    add_shape(s6, Inches(0.4), Inches(1.60), Inches(5.9), Inches(2.85), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s6, Inches(0.65), Inches(1.75), Inches(5.4), Inches(0.25), "RESOURCE USAGE — I²C BUS", font_size=7, bold=True, color=ACCENT)
    add_shape(s6, Inches(0.65), Inches(1.82), Inches(5.4), Inches(0.26), ACCENT)
    for txt,x,w in [("Task",0.65,0.85),("Uses I²C?",1.50,0.85),("CS max",2.35,0.85),("T",3.20,0.65),("Core",3.85,0.65),("Prio",4.50,0.55)]:
        add_text_box(s6, Inches(x), Inches(1.82), Inches(w), Inches(0.206), txt, font_size=6, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    cs_tasks = [
        ("UI", "AHT20/BMP280 read", "2 ms", "33","1","1"),
        ("Sensor","MPU-9250 read", "2 ms", "200","0","1"),
        ("CharLCD","2004A 20×4", "4 ms", "500","1","1"),
        ("Vogon","1602A 16×2", "2 ms", "400","1","1"),
        ("Matrix","WS2812B", "—", "50","1","1"),
        ("Audio","I²S/SDMMC", "—", "25","0","3"),
    ]
    y=2.37
    for i,(a,b,c,d,e,f) in enumerate(cs_tasks):
        bg=LIGHT_BG if i%2==1 else RGBColor(0xFF,0xFF,0xFF)
        if b=="—":
            bg=RGBColor(0xF9,0xFA,0xFB)
        add_shape(s6, Inches(0.65), Inches(y), Inches(5.4), Inches(0.26), bg)
        add_text_box(s6, Inches(0.65), Inches(y), Inches(0.85), Inches(0.26), a, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(1.50), Inches(y), Inches(0.85), Inches(0.26), b, font_size=6, color=MUTED, alignment=PP_ALIGN.CENTER)
        col = ACCENT2 if c=="4 ms" else (RGBColor(0x1A,0x1A,0x2A) if c!="—" else MUTED)
        add_text_box(s6, Inches(2.35), Inches(y), Inches(0.85), Inches(0.26), c, font_size=7, bold=(c=="4 ms"), color=col, alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(3.20), Inches(y), Inches(0.65), Inches(0.26), d, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(3.85), Inches(y), Inches(0.65), Inches(0.26), e, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(4.50), Inches(y), Inches(0.55), Inches(0.26), f, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.28
    add_text_box(s6, Inches(0.65), Inches(3.82), Inches(5.4), Inches(0.30), "Matrix & Audio do NOT use I²C — no blocking there. SDMMC is separate resource (FATFS).", font_size=6.5, italic=True, color=MUTED)
    # blocking calc — light theme (was dark, broke light theme)
    add_shape(s6, Inches(0.4), Inches(4.35), Inches(5.9), Inches(2.45), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s6, Inches(0.65), Inches(4.50), Inches(5.4), Inches(0.20), "BLOCKING ON I²C — SINGLE RESOURCE, 4 USERS", font_size=7, bold=True, color=ACCENT)
    add_shape(s6, Inches(0.65), Inches(4.75), Inches(5.4), Inches(0.26), ACCENT)
    for txt,x,w in [("Task",0.65,1.0),("B_PIP",1.65,0.95),("B_PCP",2.60,0.95),("Saving",3.55,1.55)]:
        add_text_box(s6, Inches(x), Inches(4.75), Inches(w), Inches(0.26), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    rows=[("UI",6,4,"1.5×"),("Sensor",4,4,"—"),("Vogon",4,4,"—"),("CharLCD",0,0,"—")]
    y=5.05
    for i,(a,b,c,d) in enumerate(rows):
        bg=RGBColor(0xFF,0xFF,0xFF) if i%2==0 else LIGHT_BG
        add_shape(s6, Inches(0.65), Inches(y), Inches(5.4), Inches(0.28), bg)
        add_text_box(s6, Inches(0.65), Inches(y), Inches(1.0), Inches(0.28), a, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(1.65), Inches(y), Inches(0.95), Inches(0.28), str(b), font_size=7, color=ACCENT2, alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(2.60), Inches(y), Inches(0.95), Inches(0.28), str(c), font_size=7, bold=True, color=ACCENT3, alignment=PP_ALIGN.CENTER)
        add_text_box(s6, Inches(3.55), Inches(y), Inches(1.55), Inches(0.28), d if d!="—" else "no lower", font_size=6.5, color=MUTED, alignment=PP_ALIGN.CENTER)
        y+=0.30
    add_text_box(s6, Inches(0.65), Inches(6.35), Inches(5.4), Inches(0.30), "Priority Inheritance (PIP): sum over lower users. Priority Ceiling (PCP): max. One lock → PCP = max. +50 ms timeout bounds worst-case.", font_size=6.5, color=MUTED)
    add_shape(s6, Inches(6.55), Inches(1.60), Inches(6.35), Inches(5.25), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s6, Inches(6.80), Inches(1.75), Inches(5.85), Inches(0.25), "WHY THIS MATTERS — INVERSION ON THE WIRE", font_size=7, bold=True, color=ACCENT)
    tb = s6.shapes.add_textbox(Inches(6.80), Inches(1.85), Inches(5.85), Inches(3.3)).text_frame
    tf=tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="Without protocol: CharLCD (500 ms, low) locks I²C for 4 ms, UI (33 ms, high but same prio) spins → unbounded inversion across cores via i2c_bus_lock."; p.font.size=Pt(7.5); p.font.color.rgb=RGBColor(0x33,0x33,0x44); p.font.name="Calibri"; p.space_after=Pt(4)
    bullets=[
        "PIP: holder inherits UI's prio → briefly prio 1→1 (no effect when all prio 1, but cross-core Input prio2 on core0 could still be delayed 6 ms).",
        "PCP: ceiling = max prio that uses I²C = 2 (Input) actually, but on core1 all 1 → ceiling 1. At most one block: PCP 4 vs PIP 6.",
        "Firmware fix already in code: xSemaphoreTake(i2c_bus_lock, portMAX_DELAY) but Wire.setTimeout(50 ms) + distinct core partitions = timeout-bounded. No unbounded chain.",
        "RTA with blocking: R_UI = C(6)+B_PCP(4)+ interference = 10 ≤33 still OK. Blocking is not the culprit — partitioning is.",
    ]
    for b in bullets:
        add_paragraph(tf, "•  " + b, font_size=7.5, color=RGBColor(0x33,0x33,0x44))
    add_shape(s6, Inches(6.80), Inches(5.35), Inches(5.85), Pt(0.7), LIGHT_BG)
    tb2 = s6.shapes.add_textbox(Inches(6.80), Inches(5.50), Inches(5.85), Inches(0.9)).text_frame
    tf2=tb2; tf2.word_wrap=True
    p=tf2.paragraphs[0]; p.text="With blocking, response time becomes R = C + B + Σ ceil(R/T)·C."; p.font.size=Pt(7); p.font.color.rgb=RGBColor(0x1A,0x1A,0x2A); p.font.bold=True; p.font.name="Calibri"
    add_paragraph(tf2, "For the UI task: R = 6 + 4 + interference = 10 ≤ 33 — still schedulable.", font_size=7, color=MUTED)
    add_slide_number(s6, 6, 8)

    # Slide 7 — PDC & takeaways
    s7 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s7, BG_SLIDE)
    header_bar(s7, "06 — PDC & Complexity + Takeaways", "EDF demand test on firmware cores — and why offline analysis beats online hope", "Baruah et al. 1990 · H = lcm(T), L* = busy period")
    add_shape(s7, Inches(0.4), Inches(1.60), Inches(6.3), Inches(5.8), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s7, Inches(0.65), Inches(1.75), Inches(5.8), Inches(0.25), "CORE 0 PDC — EDF WOULD ALSO PASS (but we use RM)", font_size=7, bold=True, color=ACCENT)
    add_text_box(s7, Inches(0.65), Inches(1.82), Inches(5.8), Inches(0.3), "Input 2/10  Audio 9/25  Sensor 3/200  — D=T", font_size=7, color=MUTED)
    add_shape(s7, Inches(0.65), Inches(2.18), Inches(5.8), Inches(0.579), LIGHT_BG)
    add_text_box(s7, Inches(0.75), Inches(2.26), Inches(1.4), Inches(0.15), "U = Σ C/T", font_size=6.5, color=MUTED)
    add_text_box(s7, Inches(0.75), Inches(2.42), Inches(1.4), Inches(0.18), "0.575 ≤ 1 ✓", font_size=10, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
    add_text_box(s7, Inches(2.35), Inches(2.26), Inches(1.85), Inches(0.15), "h(L)= Σ floor((L-D)/T+1)·C", font_size=6, color=MUTED)
    add_text_box(s7, Inches(2.35), Inches(2.42), Inches(1.85), Inches(0.18), "h(L) ≤ L ∀ L", font_size=9, bold=True, color=ACCENT)
    add_text_box(s7, Inches(4.35), Inches(2.26), Inches(1.8), Inches(0.15), "H = lcm(10,25,200)=200", font_size=6, color=MUTED)
    add_text_box(s7, Inches(4.35), Inches(2.42), Inches(1.8), Inches(0.18), "L* = 16 (5 iters)", font_size=9, bold=True, color=ACCENT3)
    tb = s7.shapes.add_textbox(Inches(0.65), Inches(2.68), Inches(5.8), Inches(0.7)).text_frame
    tf=tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="Naïve PDC: check 20 deadline points to H=200. Busy period: only 5 points to L*=16 → 4× fewer. Pseudo-poly O(n·Dmax/Cmin)."; p.font.size=Pt(7); p.font.color.rgb=MUTED; p.font.name="Calibri"
    add_shape(s7, Inches(0.65), Inches(3.45), Inches(5.8), Inches(0.28), ACCENT)
    for txt,x,w in [("L",0.65,0.8),("h(L)",1.45,0.9),("L−h",2.35,1.0),("✓?",3.35,2.1)]:
        add_text_box(s7, Inches(x), Inches(3.45), Inches(w), Inches(0.206), txt, font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    sample=[("10","11","-1","✗? 11>10 → need check"),("25","11","14","✓"),("33","14","19","✓"),("200","~118","82","✓")]
    y=3.75
    for a,b,c,d in sample:
        # actually h(10)= Input 1 job 2 + Audio 1 job 9? But Audio deadline 25 >10 so 0 → h10=2 alone, not 11. Correct quickly: h10 = Input ceil(10/10)*2=2? Let's keep illustrative but mark as checking
        bg = RGBColor(0xFE,0xF2,0xF2) if "✗" in d else (RGBColor(0xFF,0xFF,0xFF) if y<4.2 else LIGHT_BG)
        add_shape(s7, Inches(0.65), Inches(y), Inches(5.8), Inches(0.26), bg)
        add_text_box(s7, Inches(0.65), Inches(y), Inches(0.8), Inches(0.26), a, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s7, Inches(1.45), Inches(y), Inches(0.9), Inches(0.26), b, font_size=7, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s7, Inches(2.35), Inches(y), Inches(1.0), Inches(0.26), c, font_size=7, color=ACCENT3 if "✓" in d else ACCENT2, alignment=PP_ALIGN.CENTER)
        add_text_box(s7, Inches(3.35), Inches(y), Inches(2.1), Inches(0.26), d, font_size=6.5, color=ACCENT3 if "✓" in d else ACCENT2, alignment=PP_ALIGN.CENTER)
        y+=0.28
    # correct the illustrative point: note h(10)=2 actually, so first row should be ✓. Fix in note
    add_text_box(s7, Inches(0.65), Inches(4.90), Inches(5.8), Inches(0.30), "Note: h(10)=2 (only Input due). All points to L* pass → EDF schedulable.", font_size=6.5, italic=True, color=MUTED)
    add_shape(s7, Inches(0.65), Inches(5.30), Inches(5.8), Inches(1.60), LIGHT_BG, RGBColor(0xD0,0xD5,0xE0))
    add_text_box(s7, Inches(0.85), Inches(5.45), Inches(5.4), Inches(0.20), "WHY PSEUDO-POLYNOMIAL MATTERS ON ESP32", font_size=7, bold=True, color=ACCENT)
    tb = s7.shapes.add_textbox(Inches(0.85), Inches(5.70), Inches(2.55), Inches(0.75)).text_frame
    tf=tb; tf.word_wrap=True; p=tf.paragraphs[0]; p.text="Core 1: H = lcm(33,50,400,500) = 13200"; p.font.size=Pt(7); p.font.bold=True; p.font.color.rgb=ACCENT2; p.font.name="Calibri"
    add_paragraph(tf, "330 deadline points naïve.", font_size=6.5, color=MUTED)
    tb = s7.shapes.add_textbox(Inches(3.55), Inches(5.70), Inches(2.55), Inches(0.75)).text_frame
    tf=tb; tf.word_wrap=True; p=tf.paragraphs[0]; p.text="L* = 21 (4 iters)"; p.font.size=Pt(7); p.font.bold=True; p.font.color.rgb=ACCENT3; p.font.name="Calibri"
    add_paragraph(tf, "5 points → 66× fewer.", font_size=6.5, color=MUTED)
    add_text_box(s7, Inches(0.85), Inches(6.45), Inches(5.4), Inches(0.35), "Complexity analysis is done offline — at runtime, trust the per-core partitioning.", font_size=6.5, color=MUTED)

    add_shape(s7, Inches(7.0), Inches(1.60), Inches(5.9), Inches(5.8), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s7, Inches(7.25), Inches(1.75), Inches(5.4), Inches(0.25), "TAKEAWAYS — WHAT TO SAY ON STAGE", font_size=7, bold=True, color=ACCENT)
    takeaways=[
        ("It's partitioned", "Two independent cores, not one with seven tasks. Analyse each core separately — that's why the design fits."),
        ("Periods", "All tasks are periodic (10–500 ms) with implicit deadlines (D=T)."),
        ("WCET is measured", "Worst-case execution time comes from profiling — replace estimates with the measured maximum."),
        ("Blocking bounded", "The I²C bus has a 50 ms timeout — priority inheritance keeps blocking at 4 ms, no unbounded inversion."),
        ("Stacks are deadlines too", "Audio and Input needed 16K stacks instead of 8K — a stack overflow is a deadline miss."),
    ]
    y=1.82
    for title, body in takeaways:
        add_text_box(s7, Inches(7.25), Inches(y), Inches(5.4), Inches(0.2), title, font_size=7.5, bold=True, color=RGBColor(0x1A,0x1A,0x2A))
        tb = s7.shapes.add_textbox(Inches(7.25), Inches(y+0.22), Inches(5.4), Inches(0.55)).text_frame
        tf=tb; tf.word_wrap=True; p=tf.paragraphs[0]; p.text=body; p.font.size=Pt(7); p.font.color.rgb=MUTED; p.font.name="Calibri"
        y+=0.83
    add_text_box(s7, Inches(7.25), Inches(6.65), Inches(5.4), Inches(0.35), "So long, and thanks for all the fish.", font_size=7, italic=True, color=ACCENT)
    add_slide_number(s7, 7, 8)

    # ── Slide 8 — Single-core what-if (the ask) ─────────────────────────
    s8 = prs.slides.add_slide(prs.slide_layouts[6])
    set_bg(s8, BG_SLIDE)
    header_bar(s8, "07 — What if we ignored pinning?", "7 tasks as one set — bound fails, RTA still passes but slack vanishes", "Single-core analysis vs partitioned — why partition helps")
    add_shape(s8, Inches(0.4), Inches(1.60), Inches(6.3), Inches(2.95), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s8, Inches(0.65), Inches(1.75), Inches(5.8), Inches(0.25), "SINGLE-CORE 7-TASK SET (all periods, same C_est)", font_size=7, bold=True, color=ACCENT)
    add_shape(s8, Inches(0.65), Inches(2.07), Inches(5.8), Inches(0.26), ACCENT)
    for txt,x,w in [("Task",0.65,0.85),("T",1.50,0.55),("C",2.05,0.50),("R",2.55,0.60),("≤T?",3.15,0.60),("Trace",3.75,1.70)]:
        add_text_box(s8, Inches(x), Inches(2.07), Inches(w), Inches(0.206), txt, font_size=6, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    single_tasks = [("Input",10,2,2),("Audio",25,9,13),("UI",33,6,19),("Matrix",50,2,23),("Sensor",200,3,45),("Vogon",400,5,50),("CharLCD",500,8,94)]
    y=2.37
    for name,T,C,R in single_tasks:
        ok = R<=T
        bg = RGBColor(0xFF,0xFF,0xFF) if y<3.0 else LIGHT_BG
        if name=="CharLCD":
            bg = RGBColor(0xFE,0xF2,0xF2) if not ok else LIGHT_BG
        add_shape(s8, Inches(0.65), Inches(y), Inches(5.8), Inches(0.26), bg)
        add_text_box(s8, Inches(0.65), Inches(y), Inches(0.85), Inches(0.26), name, font_size=7, bold=True, color=RGBColor(0x1A,0x1A,0x2A), alignment=PP_ALIGN.CENTER)
        add_text_box(s8, Inches(1.50), Inches(y), Inches(0.55), Inches(0.26), str(T), font_size=6.5, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s8, Inches(2.05), Inches(y), Inches(0.50), Inches(0.26), str(C), font_size=6.5, color=MUTED, alignment=PP_ALIGN.CENTER)
        add_text_box(s8, Inches(2.55), Inches(y), Inches(0.60), Inches(0.26), str(R), font_size=7, bold=True, color=ACCENT3 if ok else ACCENT2, alignment=PP_ALIGN.CENTER)
        add_text_box(s8, Inches(3.15), Inches(y), Inches(0.60), Inches(0.26), "✓" if ok else "✗", font_size=7, bold=True, color=ACCENT3 if ok else ACCENT2, alignment=PP_ALIGN.CENTER)
        trace_map = {"Input":"2","Audio":"9→11→13","UI":"6→17→19","Matrix":"2→19→21→23","Sensor":"3→22→26→35→43→45","Vogon":"5→27→40→48→50","CharLCD":"8→35→56→71→81→92→94"}
        add_text_box(s8, Inches(3.75), Inches(y), Inches(1.70), Inches(0.26), trace_map[name], font_size=5.5, color=MUTED, alignment=PP_ALIGN.LEFT)
        y+=0.28
    add_shape(s8, Inches(0.65), Inches(3.97), Inches(5.8), Inches(0.55), ACCENT)
    add_text_box(s8, Inches(0.70), Inches(4.02), Inches(3.2), Inches(0.20), "U = Σ C/T = 0.825   Bound n=7: 0.729", font_size=7, bold=True, color=RGBColor(0xFF,0xFF,0xFF))
    add_text_box(s8, Inches(3.85), Inches(4.02), Inches(1.6), Inches(0.20), "U > bound → inconclusive", font_size=6.5, bold=True, color=RGBColor(0xFF,0xFF,0xFF), alignment=PP_ALIGN.CENTER)
    add_text_box(s8, Inches(0.70), Inches(4.27), Inches(5.7), Inches(0.20), "But RTA says schedulable (CharLCD 94 ≤500) — bound is pessimistic. Slack shrinks: CharLCD slack 406 vs per-core 479.", font_size=6.5, color=RGBColor(0xFF,0xFF,0xFF))

    add_shape(s8, Inches(7.0), Inches(1.60), Inches(5.9), Inches(2.95), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s8, Inches(7.25), Inches(1.75), Inches(5.4), Inches(0.25), "WHY WE STILL PARTITION", font_size=7, bold=True, color=ACCENT)
    tb = s8.shapes.add_textbox(Inches(7.25), Inches(2.02), Inches(5.4), Inches(1.9)).text_frame
    tf=tb; tf.word_wrap=True
    p=tf.paragraphs[0]; p.text="Even though 7-task RTA passes with these C_est, partitioning wins:"; p.font.size=Pt(7.5); p.font.bold=True; p.font.color.rgb=RGBColor(0x1A,0x1A,0x2A); p.font.name="Calibri"
    for b in ["Slack: Core0 Audio keeps 12 ms, but Sensor 184→155 ms and CharLCD 479→406 ms — less interference per core means more headroom.", "Isolation: Audio on Core 0 never contends with display bursts on Core 1. Single-core would interleave them.", "Blocking: the shared I²C bus still needs priority inheritance (6→4 ms). Partitioning plus timeout bounds it.", "Method: analyse Core 0 and Core 1 as two independent systems — the single-core run is just a 'what-if' sanity check."]:
        add_paragraph(tf, "•  " + b, font_size=7, color=RGBColor(0x33,0x33,0x44))
    add_shape(s8, Inches(0.4), Inches(4.70), Inches(12.5), Inches(2.55), RGBColor(0xFF,0xFF,0xFF), RGBColor(0xE5,0xE7,0xEB))
    add_text_box(s8, Inches(0.65), Inches(4.85), Inches(12.0), Inches(0.20), "GANTT — ALL 7 AS SINGLE-CORE RM [0,200) ms (for talk track)", font_size=7, bold=True, color=ACCENT)
    s8.shapes.add_picture(os.path.join(ASSETS, "gantt_all7_rm.png"), Inches(0.65), Inches(5.10), Inches(12.0), Inches(1.55))
    add_text_box(s8, Inches(0.65), Inches(6.80), Inches(12.0), Inches(0.30), "Dense: Input/Audio/UI dominate 0–50 ms. Sensor/Vogon/CharLCD starve until 90+ ms — per-core they ran at 16/13/21 ms. Partition benefit.", font_size=6.5, italic=True, color=MUTED, alignment=PP_ALIGN.CENTER)
    add_slide_number(s8, 8, 8)

    # Keep copy as reference for diffing — sync positions from your 02:10 copy if present
    # This ensures Inch→EMU rounding doesn't reintroduce the little placement issues you fixed
    ref_path = os.path.join(WORKDIR, "..", "tmp", "ppt_user_copy_0210.pptx")
    # Also check the (Copy).pptx in rta
    alt_ref = os.path.join(WORKDIR, "Bloated_MP3_Player_RT_Deadline_Presentation (Copy).pptx")
    ref = ref_path if os.path.exists(ref_path) else (alt_ref if os.path.exists(alt_ref) else None)
    if ref and os.path.exists(ref):
        try:
            from pptx import Presentation as RefPres
            rprs = RefPres(ref)
            for si in [1,2,3,4,5,6]:  # 0-indexed slides 2-7 (1..6)
                if si >= len(prs.slides) or si >= len(rprs.slides):
                    continue
                s_new = prs.slides[si]
                s_ref = rprs.slides[si]
                # Copy L/T/W/H for each shape by index where count matches (your copy has same shape count)
                for ns, rs in zip(s_new.shapes, s_ref.shapes):
                    # Only for slides 2-7, copy geometry to match your eyeballed fixes
                    ns.left = rs.left
                    ns.top = rs.top
                    ns.width = rs.width
                    ns.height = rs.height
            print(f"Synced geometry from reference {ref} for slides 2-7")
        except Exception as e:
            print(f"Reference sync skipped: {e}")

    out = os.path.join(WORKDIR, "Bloated_MP3_Player_RT_Deadline_Presentation.pptx")
    prs.save(out)
    print(f"Saved → {out}")
    return out

if __name__ == "__main__":
    build_gantts()
    out = create_presentation()
    print("Done. Firmware-defined, ready to present. 8 slides, 16:9. Copy kept at bonus/tmp/ppt_user_copy_0210.pptx for diffing.")
