# Pixel-Ripper

**Unified ASCII Rendering Engine** — convert images & videos into expressive ASCII-art at blazing speed, powered by SIMD, multithreading and FFmpeg.

---

## Showcase

### Still Frames

| Sketch Style | Moody Style |
|--------------|-------------|
| ![Sketch Art](outputs/Images/result_sketch_art_2.png) | ![Moody Art](outputs/Images/result_moody_art.png) |

### Video Demo

[Watch the demo video on Google Drive](https://drive.google.com/file/d/1RPuQi3l0sIhXPNrfwNgFWneqjMGc2mMQ/view?usp=sharing)

> Tip: If the link doesn’t preview in your browser, click the ⋯ menu → “Open in new window” or download it locally.

---

## Quick Start

```bash
# 1) Build the engine (requires GCC & FFmpeg dev libs)
make

# 2) Render an image directly to your terminal
./ascii_engine samples/Test1.jpg --width 120 --edge 0.3 --fit-terminal

# 3) Save an ASCII-art PNG
./ascii_engine samples/Test2.jpg --output outputs/Images/out.png --brightness 1.2 --saturate 1.0

# 4) Transcode a video to ASCII-art MP4 (CRF 23 by default)
./ascii_engine samples/Test3.mp4 --crf 23 --output outputs/Video/out.mp4
```

### Most Useful Options

| Flag | Purpose | Example |
|------|---------|---------|
| `--width <n>` | Character width of output | `--width 160` |
| `--fit-terminal` | Auto-fits width to current terminal | `--fit-terminal` |
| `--edge <f>` | Edge detection strength (0–1) | `--edge 0.4` |
| `--brightness <f>` | Brightness multiplier | `--brightness 1.3` |
| `--saturate <f>` | Saturation multiplier | `--saturate 1.1` |
| `--threads <n>` | Number of CPU threads (0 = auto) | `--threads 8` |
| `--crf <n>` | Video quality for encoded MP4 (0–51) | `--crf 18` |
| `--no-simd` | Disable SIMD acceleration | `--no-simd` |

Run the executable with no arguments to print the full help menu.

---

## Build From Source

Dependencies:

* **GCC** with support for `-march=native`
* **FFmpeg** development libraries: `libavcodec`, `libavformat`, `libswscale`, `libavutil`
* **pthread** (POSIX threads)

```bash
sudo apt install build-essential ffmpeg libavcodec-dev libavformat-dev libswscale-dev libavutil-dev
make          # builds src/*.c into the ascii_engine binary
```

Clean artifacts with:

```bash
make clean
```

---

## How It Works

1. FFmpeg decodes each frame (image or video).
2. Optional SIMD-accelerated preprocessing (edge detection, brightness, saturation).
3. Pixels are mapped to ASCII glyphs based on luminance & color.
4. Final frames are printed to console, written to PNG, or re-encoded to MP4.

See `include/ascii_engine.h` for configuration knobs.

---

## Contributing

Pull requests & issues are welcome! Feel free to open a discussion for new features or performance tricks.

---

## License

MIT — see individual header files for details.