# Changelog — Bloated MP3 Player RTA Presentation

All changes to `bonus/rta/` presentation are tracked here. No firmware (`platformio/`, `lib/`, `src/`) is modified — only `bonus/rta/generate_presentation.py` and its outputs.

## 2026-09-17 — Final review: numbering & formula (per your last points)
- Slide numbers: re-enabled subtle `1 / 8 … 8 / 8` at `12.4,7.15` (outside content) — you noted preferable, common ground stays PowerPoint UI 1..8.
- Formula: raw `Σ_{h∈hp(i)}` with `_{` → `Σ(h∈hp(i))` parenthesized; `Rₖ/Cₖ` → `Rᵢ/Cᵢ` (task `i`, iteration `k`) — now `Rᵢ⁰ = Cᵢ  Rᵢᵏ⁺¹ = Cᵢ + Σ(h∈hp(i)) ⌈Rᵢᵏ/Tₕ⌉·Cₕ`.
- Slide 8 table headers/footers `2A3455`/`1A1A2A` black → `0078D4` blue per your note.

## 2026-09-17 — Fix layout: titles, padding, theme
- Fixed title boxes overlapping content (header `title` width `9`→`12.5`, height `0.45`→`0.60`, `subtitle` y `0.9`→`1.10`, top blocks `1.35`→`1.60`, header rows `1.75/1.85`→`2.02/2.12`, data rows `2.03/2.15`→`2.32/2.42`).
- Moved slide numbers from `9.2,5.3` (inside 2nd white block) to `12.4,7.15` outside content blocks — now removed entirely (use PowerPoint's View → Slide Numbers per review).
- Fixed slide 7 inner padding for `U = Σ C/T` box (height `0.38`→`0.48`, text y `2.22`→`2.26/2.42`) — you’d already patched it, matched your fix.
- Fixed slide 8 first-box bleed into 3rd box (first `6.3×2.75`→`2.95` bottom `4.35`→`4.55`, gantt `4.35`→`4.70`, height `2.90`→`2.55` with gap).
- Fixed dark/light clash: `slide 3` bottom RTA formula huge dark `RGB 0x1A` → `LIGHT_BG #F2F4F8` with `ACCENT` header, dark text — no longer catches eye first. Slide 6 `4.35×2.25` dark → light, slide 7 `5.30×1.60` dark → light.

## 2026-09-17 — Review fixes (placement & content)
- Slide 1: added EC11 clicky pot mention (“Turn the EC11 clicky pot to browse…”) alongside HC-SR04.
- Abbreviations: introduced full forms on first use — Worst-Case Execution Time (WCET), Response Time Analysis (RTA), Rate Monotonic (RM), Priority Inheritance/Ceiling (PIP/PCP), Processor Demand Criterion (PDC), Utilization (U), Deadline (D)=Period (T).
- Slide 2 takeaway: added asterisk `* Takeaway: analyse Core 0 and Core 1 separately…` to mark disclaimer.
- Slide 3: split `R_i⁰ = C_i` / `R_i^{k+1}=…` into two-line format, removed “HOW TO READ” from slide (moved to notes `s3.notes_slide`), fixed inner padding and alignment.
- Tables: ensure row grouping — `add_shape` then `add_text_box` per row, text stays foreground.
- Blue top line: `header_bar` `Pt(4)`→`Pt(1.5)` subtle accent (not progress bar).
- Slide numbers: disabled custom `add_slide_number` — use PowerPoint's built-in numbers per review.

## 2026-09-17 — Cleanup: remove provenance & build notes from deck
- Removed all “how data was fetched” traces so deck reads as your own work: `FROM CODE`, `my_tasks_*.cpp:line`, `config.hpp`, `lib/profiling`, `PROFILE_BLOCK`, `pdMS_TO_TICKS`, `priorities.hpp`, `bonus/rta`, `tasks_raw/rta_solver` footnotes.
- Rephrased slides to present results, not methodology provenance (e.g., “All periods are pdMS_TO_TICKS in code” → “Dual-core partition: Core 0 handles audio…”).
- Verified with `pptx` grep — 0 residues remaining.

## 2026-09-17 — Cleanup: remove build notes from deck
- Removed rebuild/repro footnotes from slides (`slide 1: bonus/rta/lenv ... rebuild: ...` and `slide 7: Repro: ...`).
- Deck now contains only talk-track content; build instructions live here and in README.
- Re-exported `Bloated_MP3_Player_RT_Deadline_Presentation.pptx` (8 slides) + `pdf`.

## 2026-09-17 — Add slide 8: single-core what-if
- Added `slide 8 — What if we ignored pinning?` to illustrate `tasks_raw = ALL7` single-core RM.
- `ALL7`: Input 10/2, Audio 25/9, UI 33/6, Matrix 50/2, Sensor 200/3, Vogon 400/5, CharLCD 500/8 → `U=0.825 > bound 0.729` but RTA `CharLCD 94≤500` ✓.
- Generated `assets/gantt_all7_rm.png` window [0,200) ms, shows 0–50 ms contention vs per-core `gantt_core0/1_rm.png`.
- Shows why partition helps even though single-core RTA still passes with `C_est`.

## 2026-09-17 — Firmware-true rewrite (no placeholders)
- Replaced exam placeholder sets (`b 2/8 c 3/10 d 4/18 a 7/40`, `τ1 3/10` etc. from `rta_solver.py:6`, `q5_edf_test.py:24`) with firmware-defined values:
  - `T` from `pdMS_TO_TICKS` in `src/my_tasks_input.cpp:205` (10), `my_tasks_audio.cpp:40` (25), `my_tasks_ui.cpp:145` (33), `my_tasks_matrix.cpp:39` (50), `my_tasks_sensor.cpp:42` (200), `include/my/config.hpp:88` (CharLCD 500), `include/my/config.hpp:96` (Vogon 400).
  - `Prio/Core/Stack` from `include/my/config/priorities.hpp:18` (Audio 3/core0/16384, Input 2/core0/16384, rest 1).
  - `C` not `#define`d in firmware — labelled `C_est` from `lib/profiling` `PROFILE_BLOCK` worst-path estimate, to be replaced by measured max from `bonus/flamegraph/flamegraph.py`.
- Switched to per-core analysis: Core0 `Input>Audio>Sensor` (`U=0.575<0.780`, RTA `13≤25`, `16≤200`), Core1 `UI>Matrix>Vogon>CharLCD` (`U=0.251<0.757`, RTA `8≤50`, `21≤500`).
- I2C blocking now `SDA2/SCL1` single resource with CS 2–4 ms + `delays.hpp:43` `I2C_TRANSACTION_TIMEOUT_MS=50`, `PIP 6→PCP 4`, not exam `R1–R5` matrix (`blocking_times.py:18`).
- New Gantts `gantt_core0_rm.png`, `gantt_core1_rm.png`, `gantt_core0_edf.png`.

## 2026-09-16 — Initial deck (exam-based, 7 slides)
- Created `bonus/rta/lenv/` (python-pptx, matplotlib, pillow) and `generate_presentation.py` (7 slides, 16:9) from `bonus/rta/*.py` calculators:
  - Task set `b/c/d/a` (Q3), EDF `τ1–τ4` (Q5/Q6), `R1–R5` (Q4) with Gantts `gantt_rm/edf(_np).png`.
  - Exported `Bloated_MP3_Player_RT_Deadline_Presentation.pptx` + `pdf` via LibreOffice.

## Rebuild
```bash
bonus/rta/lenv/bin/python bonus/rta/generate_presentation.py
libreoffice --headless --convert-to pdf --outdir bonus/rta bonus/rta/Bloated_MP3_Player_RT_Deadline_Presentation.pptx
```
