/*
 * =====================================================================================
 *
 * Filename:  ascii_engine.h
 *
 * =====================================================================================
 */

#ifndef ASCII_ENGINE_H
#define ASCII_ENGINE_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

// Why forward-declare? To keep this header clean and self-contained. It acts as
// a contract for the engine's API without exposing the internal chaos of FFmpeg's
// own headers to the consumer of our library.
struct AVFormatContext;
struct AVCodecContext;
struct SwsContext;
struct AVFrame;
struct AVPacket;

typedef enum {
    MODE_IMAGE,
    MODE_VIDEO,
    MODE_ANIMATED_GIF // Explicitly handle GIFs to accommodate their unique timing.
} ProcessingMode;

typedef enum {
    DITHER_NONE,
    DITHER_FLOYD
} DitherMode;

typedef struct {
    ProcessingMode mode;
    int output_width;
    float edge_strength;
    float aspect_correction;
    float brightness_factor;
    float saturation_factor;
    int use_color;
    char* output_filename;
    int num_threads;
    DitherMode dither_mode;
    int use_simd;
    int crf; // Constant Rate Factor: Direct control over the soul of the video encoder.
    
    // V17 GOD-TIER features
    int use_haruhi;      // 0=base engine, 1=HARUHI v9.0
    int use_hdr;         // Enable HDR tone mapping
    float glow_strength; // Glow/bloom effect strength (0=off)
    float skin_smooth;   // Skin detection smoothing (0=off, 1.0=normal)
    
    // V18 TRANSCENDENCE features
    float crt_strength;     // CRT phosphor effect (0=off)
    float scanline_strength;// Scanline darkness (0=off, 1.0=full)
    int color_grade;        // Color grading: 0=none, 1=cinematic, 2=noir, 3=vintage
    
    // V20 BETA features
    int pure_mode;          // Skip all color processing, use raw source colors
    int dither_type;        // 0=none, 1=floyd-steinberg, 2=bayer, 3=ordered
    int edge_only;          // Only render edges/outlines
    int pixelate_size;      // Pixel block size (0=off)
    int ramp_mode;          // BVOID ramp: 0=default, 1=blocks, 5=unicode, 6=braille
} EngineConfig;

typedef struct ProcessingContext ProcessingContext;

ProcessingContext* engine_init(const char* input_source, const EngineConfig* config, char** error);
void engine_cleanup(ProcessingContext** ctx);

int engine_get_next_packet(ProcessingContext* ctx, struct AVPacket* packet);
int engine_decode_video_packet(ProcessingContext* ctx, struct AVPacket* packet, struct AVFrame** frame);
void engine_process_frame_to_ascii(ProcessingContext* ctx, const struct AVFrame* frame, const EngineConfig* config);

void engine_render_to_console(ProcessingContext* ctx, const EngineConfig* config);
int engine_render_to_image_file(ProcessingContext* ctx, const EngineConfig* config);
int engine_render_to_svg_file(ProcessingContext* ctx, const EngineConfig* config);

// V18: New export formats
int engine_render_to_ansi_file(ProcessingContext* ctx, const EngineConfig* config);
int engine_render_to_html_file(ProcessingContext* ctx, const EngineConfig* config);

int engine_encode_video_frame(ProcessingContext* ctx, const struct AVFrame* original_frame, const EngineConfig* config);
int engine_remux_packet(ProcessingContext* ctx, struct AVPacket* packet);
void engine_finalize_video_encoder(ProcessingContext* ctx);

// Why return a double? Frame timings can be precise; we use microseconds for usleep,
// but returning a double gives the caller flexibility. This is now more important
// for handling variable frame rates in formats like GIF.
double engine_get_frame_delay_secs(const ProcessingContext* ctx, const struct AVFrame* frame);
int engine_get_video_stream_idx(const ProcessingContext* ctx);
int engine_get_audio_stream_idx(const ProcessingContext* ctx);
float engine_get_video_aspect(const ProcessingContext* ctx);
void engine_update_output_dims(ProcessingContext* ctx, int new_ascii_width, int new_ascii_height);

int is_animated_file(const char* filename);

#endif // ASCII_ENGINE_H

