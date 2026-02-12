/*
 * =====================================================================================
 *
 * Filename:  main.c
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>

#include "ascii_engine.h"
#include <libavcodec/avcodec.h>

// Why volatile sig_atomic_t? This is the only correct way to handle signal flags
// in C. 'volatile' prevents the compiler from optimizing away reads of the flag,
// and 'sig_atomic_t' guarantees that reads and writes to it are atomic,
// preventing race conditions between the main loop and the signal handler.
volatile sig_atomic_t terminal_resized_flag = 0;

void handle_resize_signal(int sig) {
    (void)sig;
    terminal_resized_flag = 1;
}

void hide_cursor() {
    printf("\x1b[?25l");
}

void show_cursor() {
    printf("\x1b[?25h");
}

// Why this handler? A standard signal handler must accept an integer argument.
// This function wraps our cursor-showing logic and ensures a clean exit,
// which is the expected behavior for a SIGINT (Ctrl+C).
void handle_interrupt(int sig) {
    (void)sig;
    show_cursor();
    exit(0);
}

void fit_to_terminal(ProcessingContext* ctx, const EngineConfig* config) {
    struct winsize w;
    // Why ioctl? It's the direct, low-level way to query the kernel for terminal
    // properties. This avoids parsing command output or linking to heavier
    // libraries like ncurses. It's the most direct path to the data we need.
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0 || w.ws_col <= 0 || w.ws_row <= 0) {
        return;
    }

    float video_aspect = engine_get_video_aspect(ctx);
    // Why aspect_correction? Terminal character cells are not square; they are
    // typically twice as tall as they are wide. This correction factor adjusts
    // the video's aspect ratio to match the character grid's aspect ratio,
    // preventing the output from looking stretched.
    float corrected_video_aspect = video_aspect / config->aspect_correction;
    float terminal_aspect = (float)w.ws_col / (float)w.ws_row;

    int new_width, new_height;

    if (terminal_aspect > corrected_video_aspect) {
        // Terminal is wider than the video -> height is the constraint
        new_height = w.ws_row - 1; // -1 to avoid scroll on last line
        new_width = (int)(new_height * corrected_video_aspect);
    } else {
        // Terminal is taller than the video -> width is the constraint
        new_width = w.ws_col;
        new_height = (int)(new_width / corrected_video_aspect);
    }

    engine_update_output_dims(ctx, new_width, new_height);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [options]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  --width <n>          Set output width in characters (e.g., 120)\n");
        fprintf(stderr, "  --edge <f>           Set edge detection threshold (e.g., 0.4)\n");
        fprintf(stderr, "  --output <file>      Output to file instead of console\n");
        fprintf(stderr, "  --fit-terminal       Fit width to the current terminal\n");
        fprintf(stderr, "  --brightness <f>     Brightness factor (e.g., 1.5)\n");
        fprintf(stderr, "  --saturate <f>       Saturation factor (e.g., 1.0)\n");
        fprintf(stderr, "  --threads <n>        Number of threads to use (0=auto)\n");
        fprintf(stderr, "  --crf <n>            Video quality (Constant Rate Factor, 0-51, lower is better, 18-28 is sane)\n");
        fprintf(stderr, "  --no-simd            Disable SIMD optimizations\n");
        fprintf(stderr, "  --theme <n>          HARUHI color theme (0=default, 1=cyberpunk, 2=matrix, 3=retro, 4=neon, 5=pastel, 6=mono)\n");
        fprintf(stderr, "  --quality <n>        HARUHI quality (0=fast, 1=balanced, 2=high, 3=ultra)\n");
        fprintf(stderr, "  --ramp <n>           HARUHI char ramp (0=default, 1=blocks, 3=halftone, 5=unicode, 6=braille)\n");
        fprintf(stderr, "  --filter <n>         HARUHI filter (0=none, 1=glow, 2=edge_glow, 3=neural)\n");
        fprintf(stderr, "  --perception         HARUHI v10: texture-aware perception algorithm\n");
        fprintf(stderr, "  --engine <name>      Engine: 'base' (default) or 'haruhi' (V17 neural)\n");
        fprintf(stderr, "  --hdr                Enable HDR tone mapping (prevent color clipping)\n");
        fprintf(stderr, "  --glow <f>           Bloom/glow effect strength (e.g., 1.5)\n");
        fprintf(stderr, "  --skin <f>           Skin detection smoothing (e.g., 1.0)\n");
        fprintf(stderr, "  --crt <f>            CRT phosphor glow effect (e.g., 1.0)\n");
        fprintf(stderr, "  --scanlines <f>      Retro scanline effect (e.g., 0.5)\n");
        fprintf(stderr, "  --grade <n>          Color grading: 1=cinematic, 2=noir, 3=vintage\n");
        fprintf(stderr, "  --pure               V20: Use exact original colors (no processing)\n");
        fprintf(stderr, "  --dither <n>         V20: Dithering 1=floyd-steinberg, 2=bayer, 3=ordered\n");
        fprintf(stderr, "  --edge-only          V20: Render only edges (sketch look)\n");
        fprintf(stderr, "  --pixelate <n>       V20: Pixel art mode with block size\n");
        fprintf(stderr, "  --preset <name>      V20: twitter, discord, instagram, wallpaper\n");
        fprintf(stderr, "  --ramp <n>           BVOID ramp: 0=default, 1=blocks, 5=unicode, 6=braille (BVOID engine only)\n");
        return 1;
    }

    EngineConfig config = {
        .output_width = 120,
        .edge_strength = 0.4f,
        .aspect_correction = 0.5f,
        .brightness_factor = 1.0f,
        .saturation_factor = 1.0f,
        .use_color = 1,
        .output_filename = NULL,
        .num_threads = 0,
        .dither_mode = DITHER_NONE,
        .use_simd = 1,
        .crf = 23, // A sane default for good quality and reasonable file size.
        // V17 defaults
        .use_haruhi = 0,
        .use_hdr = 0,
        .glow_strength = 0.0f,
        .skin_smooth = 0.0f,
        // V18 TRANSCENDENCE defaults
        .crt_strength = 0.0f,
        .scanline_strength = 0.0f,
        .color_grade = 0,
        // V20 BETA defaults
        .pure_mode = 0,
        .dither_type = 0,
        .edge_only = 0,
        .pixelate_size = 0,
        .ramp_mode = 0  // BVOID default ramp
    };
    int fit_terminal = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            config.output_width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--edge") == 0 && i + 1 < argc) {
            config.edge_strength = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            config.output_filename = argv[++i];
        } else if (strcmp(argv[i], "--fit-terminal") == 0) {
            fit_terminal = 1;
        } else if (strcmp(argv[i], "--brightness") == 0 && i + 1 < argc) {
            config.brightness_factor = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--saturate") == 0 && i + 1 < argc) {
            config.saturation_factor = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            config.num_threads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--crf") == 0 && i + 1 < argc) {
            config.crf = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--no-simd") == 0) {
            config.use_simd = 0;
        } else if (strcmp(argv[i], "--theme") == 0 && i + 1 < argc) {
            // HARUHI v9.0: Color themes (0=default, 1=cyberpunk, 2=matrix, 3=retro, 4=neon, 5=pastel, 6=mono)
            extern void haruhi_set_theme(int);
            haruhi_set_theme(atoi(argv[++i]));
        } else if (strcmp(argv[i], "--quality") == 0 && i + 1 < argc) {
            // HARUHI v9.0: Quality modes (0=fast, 1=balanced, 2=high, 3=ultra)
            extern void haruhi_set_quality(int);
            haruhi_set_quality(atoi(argv[++i]));
        } else if (strcmp(argv[i], "--ramp") == 0 && i + 1 < argc) {
            // Shared ramp mode for both engines.
            int ramp_mode = atoi(argv[++i]);
            extern void haruhi_set_ramp_mode(int);
            haruhi_set_ramp_mode(ramp_mode);
            config.ramp_mode = ramp_mode;
        } else if (strcmp(argv[i], "--filter") == 0 && i + 1 < argc) {
            // HARUHI v9.0: Filters (0=none, 1=glow, 2=edge_glow, 3=neural)
            extern void haruhi_set_filter(int, float);
            haruhi_set_filter(atoi(argv[++i]), 1.0f);
        } else if (strcmp(argv[i], "--perception") == 0) {
            // HARUHI v10: perception algorithm
            extern void haruhi_set_perception(int);
            haruhi_set_perception(1);
        } else if (strcmp(argv[i], "--engine") == 0 && i + 1 < argc) {
            // V17: Engine selection
            if (strcmp(argv[i+1], "haruhi") == 0) {
                config.use_haruhi = 1;
            }
            i++;
        } else if (strcmp(argv[i], "--hdr") == 0) {
            // V17: HDR tone mapping
            config.use_hdr = 1;
        } else if (strcmp(argv[i], "--glow") == 0 && i + 1 < argc) {
            // V17: Glow/bloom effect
            config.glow_strength = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--skin") == 0 && i + 1 < argc) {
            // V17: Skin detection smoothing
            config.skin_smooth = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--crt") == 0 && i + 1 < argc) {
            // V18: CRT phosphor effect
            config.crt_strength = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--scanlines") == 0 && i + 1 < argc) {
            // V18: Scanline effect
            config.scanline_strength = strtof(argv[++i], NULL);
        } else if (strcmp(argv[i], "--grade") == 0 && i + 1 < argc) {
            // V18: Color grading preset
            config.color_grade = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--pure") == 0) {
            // V20: Pure mode - no color modifications
            config.pure_mode = 1;
        } else if (strcmp(argv[i], "--dither") == 0 && i + 1 < argc) {
            // V20: Dithering mode
            config.dither_type = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--edge-only") == 0) {
            // V20: Edge-only rendering
            config.edge_only = 1;
        } else if (strcmp(argv[i], "--pixelate") == 0 && i + 1 < argc) {
            // V20: Pixelate + ASCII
            config.pixelate_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--preset") == 0 && i + 1 < argc) {
            // V20: Social media presets
            const char* preset = argv[++i];
            if (strcmp(preset, "twitter") == 0) {
                config.output_width = 62;  // ~500px at 8px/char
            } else if (strcmp(preset, "discord") == 0) {
                config.output_width = 100; // ~800px
            } else if (strcmp(preset, "instagram") == 0) {
                config.output_width = 135; // ~1080px square
            } else if (strcmp(preset, "wallpaper") == 0) {
                config.output_width = 240; // ~1920px
            }
        }
    }

    const char* input_file = argv[1];
    if (is_animated_file(input_file)) {
        if (strstr(input_file, ".gif")) {
            config.mode = MODE_ANIMATED_GIF;
        } else {
            config.mode = MODE_VIDEO;
        }
    } else {
        config.mode = MODE_IMAGE;
    }

    if (config.output_filename) {
        // Why 1.0 aspect correction for file output? Because the output is a pixel-based
        // image or video, not a character grid. Each character will be rendered into
        // an 8x8 square of pixels, so we don't need to correct for non-square
        // terminal characters.
        config.aspect_correction = 1.0f;
    }

    char* error = NULL;
    ProcessingContext* ctx = engine_init(input_file, &config, &error);
    if (!ctx) {
        fprintf(stderr, "Engine initialization failed: %s\n", error ? error : "Unknown error");
        return 1;
    }

    if (fit_terminal) {
        fit_to_terminal(ctx, &config);
        if (!config.output_filename) {
            signal(SIGWINCH, handle_resize_signal);
        }
    }

    if (!config.output_filename) {
        hide_cursor();
        // Why trap SIGINT? So we can restore the cursor if the user hits Ctrl+C.
        // Leaving the cursor hidden after exit is poor form.
        signal(SIGINT, handle_interrupt);
    }

    if (config.mode == MODE_VIDEO || config.mode == MODE_ANIMATED_GIF) {
        if (config.output_filename) {
            AVPacket* packet = av_packet_alloc();
            int frame_count = 0;
            printf("Transcoding... (Audio will be passed through)\n");
            while(engine_get_next_packet(ctx, packet) >= 0) {
                if (packet->stream_index == engine_get_video_stream_idx(ctx)) {
                    struct AVFrame* frame = NULL;
                    if (engine_decode_video_packet(ctx, packet, &frame) == 0) {
                        engine_process_frame_to_ascii(ctx, frame, &config);
                        if (engine_encode_video_frame(ctx, frame, &config) != 0) {
                            fprintf(stderr, "\nError encoding frame\n");
                            break;
                        }
                        printf("Encoded video frame %d\r", ++frame_count);
                        fflush(stdout);
                    }
                } else if (packet->stream_index == engine_get_audio_stream_idx(ctx)) {
                    if (engine_remux_packet(ctx, packet) < 0) {
                        fprintf(stderr, "\nError writing audio packet. Stopping.\n");
                        break;
                    }
                }
                av_packet_unref(packet);
            }
            engine_finalize_video_encoder(ctx);
            printf("\nFinished encoding video to %s\n", config.output_filename);
            av_packet_free(&packet);
        } else { // Real-time playback
            struct AVFrame* frame = NULL;
            AVPacket* packet = av_packet_alloc();
            while (engine_get_next_packet(ctx, packet) >= 0) {
                 if (terminal_resized_flag) {
                    printf("\x1b[2J"); // Clear screen
                    fit_to_terminal(ctx, &config);
                    terminal_resized_flag = 0;
                 }
                 if (packet->stream_index == engine_get_video_stream_idx(ctx)) {
                    if (engine_decode_video_packet(ctx, packet, &frame) == 0 && frame) {
                        long frame_delay_us = (long)(engine_get_frame_delay_secs(ctx, frame) * 1000000.0);
                        engine_process_frame_to_ascii(ctx, frame, &config);
                        engine_render_to_console(ctx, &config);
                        usleep(frame_delay_us);
                    }
                 }
                 av_packet_unref(packet);
            }
            av_packet_free(&packet);
        }
    } else { // Image mode
        struct AVFrame* frame = NULL;
        if (engine_decode_video_packet(ctx, NULL, &frame) == 0) {
            engine_process_frame_to_ascii(ctx, frame, &config);
            if (config.output_filename) {
                const char* ext = strrchr(config.output_filename, '.');
                int render_type = 0;  // 0=png, 1=svg, 2=ansi, 3=html
                
                if (ext) {
                    if (strcmp(ext, ".svg") == 0) render_type = 1;
                    else if (strcmp(ext, ".ansi") == 0 || strcmp(ext, ".ans") == 0) render_type = 2;
                    else if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) render_type = 3;
                }
                
                int result;
                switch (render_type) {
                    case 1:  result = engine_render_to_svg_file(ctx, &config); break;
                    case 2:  result = engine_render_to_ansi_file(ctx, &config); break;
                    case 3:  result = engine_render_to_html_file(ctx, &config); break;
                    default: result = engine_render_to_image_file(ctx, &config); break;
                }
                
                if (result == 0) {
                    printf("Rendered ASCII art to %s\n", config.output_filename);
                } else {
                    fprintf(stderr, "ERROR: Could not write image to disk. Check permissions or path.\n");
                }
            } else {
                engine_render_to_console(ctx, &config);
                printf("\n");
            }
        }
    }

    if (!config.output_filename) {
        show_cursor();
    }

    engine_cleanup(&ctx);
    return 0;
}
