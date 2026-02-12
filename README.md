# Pixel-Ripper

Pixel-Ripper is a C-based image/video to ASCII renderer focused on throughput and controllable output quality.

This project favors explicit performance work over abstraction layers: SIMD paths, thread-level parallelism, and arena-style allocation in hot loops.

## Core Features

- Image and video input support (FFmpeg + stb image path).
- Multiple ASCII ramps and rendering modes.
- Multithreaded frame processing with `pthreads`.
- SIMD-accelerated pixel processing (SSSE3 path).
- CLI configuration for output width, quality, filters, and themes.

## Build Dependencies

### Linux (Debian/Ubuntu)

```bash
sudo apt install build-essential libavcodec-dev libavformat-dev libswscale-dev libavutil-dev
```

### macOS

```bash
brew install ffmpeg
```

### Windows (MSYS2 / MINGW64)

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-make
```

## Build

```bash
make clean && make
```

## Run

```bash
# Basic image conversion
./ascii_engine input.png --width 200 --output out.png

# Video conversion
./ascii_engine input.mp4 --width 180 --quality 2 --output out.mp4
```

## Design Notes

- Arena allocation is used to reduce allocator churn in frame loops.
- LUTs are used to avoid repeated expensive computations.
- Thread slicing is used to spread CPU-heavy stages across cores.

## Limits

- Not all code paths are SIMD-accelerated.
- Tuning defaults vary by input type and resolution.
- This is a performance-learning codebase, not a general media framework.
