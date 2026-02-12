# Pixel-Ripper Master Commands

This file is a clean command reference for `ascii_engine`.

## Build

```bash
make clean && make
```

## Basic Usage

```bash
./ascii_engine <input_file> [options]
```

## Quick Start Commands

### 1) High-quality image output

```bash
./ascii_engine input.png \
  --engine haruhi \
  --width 500 \
  --quality 3 \
  --theme 1 \
  --brightness 1.4 \
  --saturate 1.2 \
  --edge 0.4 \
  --output masterpiece.png
```

### 2) Terminal preview (auto fit)

```bash
./ascii_engine input.png --engine haruhi --fit-terminal
```

### 3) Cyberpunk style

```bash
./ascii_engine input.png \
  --engine haruhi \
  --width 400 \
  --quality 2 \
  --theme 1 \
  --filter 1 \
  --brightness 1.6 \
  --saturate 2.0 \
  --edge 0.3 \
  --output cyberpunk.png
```

### 4) Matrix style

```bash
./ascii_engine input.png \
  --engine haruhi \
  --width 300 \
  --quality 2 \
  --theme 2 \
  --filter 2 \
  --brightness 1.2 \
  --saturate 0.0 \
  --edge 0.6 \
  --output matrix.png
```

### 5) Braille mode

```bash
./ascii_engine input.png \
  --engine haruhi \
  --width 200 \
  --quality 3 \
  --ramp 6 \
  --brightness 1.3 \
  --edge 0.25 \
  --output braille.png
```

### 6) Video conversion

```bash
./ascii_engine input.mp4 \
  --engine haruhi \
  --width 180 \
  --quality 2 \
  --crf 20 \
  --output ascii_video.mp4
```

### 7) Social preset output

```bash
./ascii_engine input.png --preset twitter --output twitter.png
./ascii_engine input.png --preset discord --output discord.png
./ascii_engine input.png --preset instagram --output instagram.png
./ascii_engine input.png --preset wallpaper --output wallpaper.png
```

## Option Reference

### Core

- `--engine <name>`: `base` or `haruhi`
- `--width <n>`: output width in characters
- `--output <file>`: output file path
- `--fit-terminal`: auto-fit to terminal size

### Quality / Visual

- `--quality <n>`: `0=fast`, `1=balanced`, `2=high`, `3=ultra`
- `--theme <n>`: `0=default`, `1=cyberpunk`, `2=matrix`, `3=retro`, `4=neon`, `5=pastel`, `6=mono`
- `--ramp <n>`: `0=default`, `1=blocks`, `3=halftone`, `5=unicode`, `6=braille`
- `--filter <n>`: `0=none`, `1=glow`, `2=edge_glow`, `3=neural`
- `--brightness <f>`: brightness multiplier
- `--saturate <f>`: saturation multiplier
- `--edge <f>`: edge threshold

### Performance / Video

- `--threads <n>`: thread count (`0=auto`)
- `--no-simd`: disable SIMD path
- `--crf <n>`: video quality (lower = better, larger file)

### Extra Effects

- `--perception`: enable texture-aware perception
- `--hdr`: enable HDR tone mapping
- `--glow <f>`: bloom strength
- `--skin <f>`: skin smoothing
- `--crt <f>`: CRT effect
- `--scanlines <f>`: scanline strength
- `--grade <n>`: color grade preset

### V20-style Modes

- `--pure`: keep source colors (minimal processing)
- `--dither <n>`: dithering mode
- `--edge-only`: edge-only rendering
- `--pixelate <n>`: pixel block size
- `--preset <name>`: social format preset

## Copy/Paste Defaults

### Balanced

```bash
./ascii_engine input.png --engine haruhi --width 300 --quality 1 --output out.png
```

### High

```bash
./ascii_engine input.png --engine haruhi --width 400 --quality 2 --output out.png
```

### Ultra

```bash
./ascii_engine input.png --engine haruhi --width 500 --quality 3 --output out.png
```
