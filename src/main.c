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
        .crf = 23 // A sane default for good quality and reasonable file size.
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
                if (engine_render_to_image_file(ctx, &config) == 0) {
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

