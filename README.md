# **DON'T PANIC**

## bloated_mp3_player

**The answer is 42. The question, it turns out, is: "How many peripherals can you bolt onto an ESP32 before the scheduler files for divorce?"**

This is a multithreaded embedded project that started life as a humble MP3 player and, through a series of unfortunate events vaguely resembling a real-time systems course, accreted so many non-essential components that it now requires a concurrent scheduler just to keep them all arguing fairly for CPU time.

It runs on an ESP32-S3 (the electronic equivalent of an Infinite Improbability Drive, only slightly less likely to turn you into a potted plant or a philosophical whale), uses FreeRTOS under the hood (courtesy of the Arduino framework), and demonstrates that you can, in fact, teach concurrency by strapping seventeen unrelated sensors to a single I²C bus and hoping for the best.


## About

> *"The ships hung in the sky in much the same way that bricks don't."*
> — Douglas Adams, *The Hitchhiker's Guide to the Galaxy*

This program is a multithreaded C++ monstrosity for the Real Time Embedded Systems course at **Unimore Enzo Ferrari AI and engeneering department**. It was inspired by *The Hitchhiker's Guide to the Galaxy* not because it does anything particularly galactic, but because the sheer quantity of peripherals tacked onto it would make even a Vogon constructor fleet raise an eyebrow.

The core mission: play MP3s from a microSD card while simultaneously monitoring temperature, humidity, air pressure, rotation, acceleration, magnetic fields, and the existential dread of a WS2812B RGB matrix trying to decide what colour to be or where to put the bablefish.

Is it practical? No.
Does it demonstrate concurrency? Absolutely.
Would Zaphod Beeblebrox approve? Almost certainly, right before stealing it.


## Disclaimer

- This program is provided **as is**, without warranty of any kind — implied, express, or Vogon-constructed. If your ESP32 achieves sentience and demands a better quality of MP3, that's your problem.
- This program was written with the assistance of AI. The AI is not responsible for any design decisions, but it did try to talk us out of the 16×16 RGB matrix. We didn't listen.
- Developed under **Ubuntu 24.04.4 LTS**, where ESP32s tend to materialise on `/dev/ttyACM0` (unless they're feeling contrary, in which case check `dmesg`).
- Targeted at the **ESP32-S3 WROOM FREENOVE** dev board. Flashed via the PlatformIO extension for VS Code (or however one normally communicates with small, irritable computers).
- The [KiCad schematic](./hardware/) is provided as a wiring reference. You could send it to a PCB manufacturer if you're feeling brave, but the author (and his AI accomplice) accept no responsibility for design issues, misplaced vias, or spontaneous combustion.
- Components are universally sourcable from websites such as Amazon, AliExpress, or (for the truly adventurous) by flagging down a passing Dentrassi supply ship.


## Dependencies

The software stack, from bottom to top:

| Layer | What it is | Why it's here |
|-------|-----------|---------------|
| FreeRTOS | Real-time OS (via Arduino) | So 17 tasks can deadlock each other in style |
| Arduino ESP32 Core | HAL / framework | Because writing raw ESP-IDF is for people with more patience |
| [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) | RGB LED control | Makes the matrix glow reassuringly |
| [U8g2](https://github.com/olikraus/u8g2) | OLED/LCD monochrome graphics | Draws text and badly-scaled album art on the 128×64 LCD |

All library dependencies are resolved automatically by PlatformIO via `platformio.ini`.


## Components Used

> *"The Guide is definitive. Reality is frequently inaccurate."*
> — *The Hitchhiker's Guide to the Galaxy*

The following have been sacrificed on the altar of concurrency:

| Purpose | Name | Price (€) | Quantity | Link |
|---------|------|-----------|----------|------|
| :brain: The brains | Freenove ESP32-S3-WROOM CAM — Dual-core 32-bit 240 MHz | 20.19 | 1 | [AliExpress](https://aliexpress.com/item/1005004960637276.html) |
| :tv: Display | Lcd12864 12864-06D — COG dot matrix, SPI, with Chinese font | 6.19 | 1 | [AliExpress](https://aliexpress.com/item/1005009024477658.html) |
| :thermometer: Weather station | AHT20+BMP280 — Temperature, humidity & air pressure | 1.20 | 1 | [AliExpress](https://aliexpress.com/item/1005007702473893.html) |
| :triangular_ruler: Distance | HC-SR04 — Ultrasonic "how far is the wall" sensor | 1.41 | 1 | [AliExpress](https://aliexpress.com/item/1005007849944952.html) |
| :arrows_counterclockwise: User input | EC11 Rotary Encoder with switch — 20 positions, knob shaft | 3.49 | 1 | [AliExpress](https://aliexpress.com/item/1005004908084537.html) |
| :loud_sound: Crappy beeps | 4Ω 3W / 8Ω 2W cavity speaker — tiny, tinny, test-tones only | 1.38 | 2 | [AliExpress](https://aliexpress.com/item/1005008626624201.html) |
| :compass: Orientation | GY-9250 MPU-9250 — 9-axis gyro + accel + magnetometer | 3.09 | 1 | [AliExpress](https://aliexpress.com/item/1005007211455522.html) |
| :flashlight: Light show | WS2812B 16×16 RGB LED matrix — individually addressable chaos | 14.39 | 1 | [AliExpress](https://aliexpress.com/item/1005005394534715.html) |
| :floppy_disk: Storage | Micro SD Card Module — SPI level-shifted | 1.24 | 1 | [AliExpress](https://aliexpress.com/item/1005010653353565.html) |

**Total carnage:** ~€52.58 plus fuses, caps, and whatever else you inevitably forgot.

For fuses, capacitors, resistors, and the 5 V → 3.3 V regulators, see the schematic. You probably have them in a drawer somewhere. The author assumes no responsibility for the state of that drawer.


## How to Flash

> *"A towel is about the most massively useful thing an interstellar hitchhiker can have."*

1. Load some `.mp3` files onto a microSD card. 44.1 kHz, 16-bit stereo is the recommended flavour of the universe.
2. Insert the card into the ESP32's SD card reader (or the module you soldered on after muttering imprecations at the pinout).
3. Open the project in VS Code with the PlatformIO extension installed. If you don't have PlatformIO, you're about to have a Very Bad Time.
4. Connect your ESP32. There are usually two USB ports: one for UART (debug) and one for OTG (flash). **The OTG one is the one you want.** If you get it wrong, nothing explodes — you'll just get a timeout. This is fine.
5. Locate the **`→`** arrow at the bottom of the VS Code window. Click it. This compiles the firmware and, if the stars align, flashes it to the board.
6. If the flash fails: check the port, hold the BOOT button, and try again. Repeat until the universe stops playing tricks on you.

### Pre-flight flash erase

A PlatformIO extra script (`middleware/full_erase.py`) automatically erases the ESP32's flash before each upload. This ensures no stale data haunts your new firmware like the ghost of a Vogon poem.

It can also be run standalone:

```
python middleware/full_erase.py                          # auto-detect port
python middleware/full_erase.py --port /dev/ttyUSB0      # explicit port
```

It will find `esptool.py` in your PATH, or tell you to `pip install esptool` if you haven't.


## How to Use

> *"In the beginning, the universe was created. This has made a lot of people very angry and been widely regarded as a bad move."*

Turn the rotary encoder to navigate the menu. Press the switch to select. The interface, such as it is, will appear on the LCD. The RGB matrix will do something presumably artistic. The ultrasonic sensor will judge your distance from the board. The MPU-9250 will ponder its orientation in the cosmos. The temperature sensor will inform you that it is, in fact, room temperature.

There is no off switch. You have been warned.


## Bonus Tools

### Flamegraph Profiling (bonus/extras/flamegraph.py)

The firmware includes a lightweight profiler that can dump task timing traces and CPU usage stats over the serial port. The `flamegraph.py` script converts that output into an interactive SVG flamegraph — a visual representation of where the CPU is spending its time (or, as the Guide would put it, "how the microprocessor is frittering away the microseconds").

**To capture profiling data:**

1. In the firmware, ensure `Profiling::dump_traces()` or `Profiling::dump_task_stats()` is called. The output appears on the serial console (typically 115200 baud) in sections bracketed by `=== PROFILING TRACES ===` / `=== END PROFILING TRACES ===`.
2. Save that serial output to a file:
   ```
   platformio device monitor --raw > serial_dump.txt
   ```
   Press the reset button to trigger the dump, then hit Ctrl+C to stop.
3. Generate the flamegraph:
   ```
   python bonus/extras/flamegraph.py serial_dump.txt
   ```
   This produces `flamegraph.svg` (or `flamegraph_traces.svg` / `flamegraph_task_stats.svg` if multiple sections are present).
4. Open the SVG in any browser. The wider a bar, the more time that function consumed. Hover for details.

**Usage summary:**

```
python bonus/extras/flamegraph.py < serial_dump.txt              # read from stdin
python bonus/extras/flamegraph.py serial_dump.txt                # read from file
python bonus/extras/flamegraph.py serial_dump.txt -o output.svg  # custom output
```

### Image & Font Conversion (bonus/ dir)

- **`bonus/img2c/convert.py`** — Convert PNG icons to C byte arrays (1-bit packed + 8-bit grayscale), with optional PlatformIO library mode.
- **`bonus/font2c/convert.py`** — Render TTF/OTF fonts to C byte arrays at configurable point sizes, with full Unicode range support and proportional glyph width tables.

See each script's `--help` for details. Both require Pillow:

```
pip install -r bonus/img2c/requirements.txt
pip install -r bonus/font2c/requirements.txt
```

If you don't want to upset the universe, use a virtual environment — your OS is the planet and the env is the cryogenised body in orbit keeping the British speaking clock off your phone bill.

## License

> *"So long, and thanks for all the fish."*

This project is licensed under the [MIT License](./LICENSE). Do what you like with it. If you build something cool, tell the dolphins.
