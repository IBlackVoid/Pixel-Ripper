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
} EngineConfig;

typedef struct ProcessingContext ProcessingContext;

ProcessingContext* engine_init(const char* input_source, const EngineConfig* config, char** error);
void engine_cleanup(ProcessingContext** ctx);

int engine_get_next_packet(ProcessingContext* ctx, struct AVPacket* packet);
int engine_decode_video_packet(ProcessingContext* ctx, struct AVPacket* packet, struct AVFrame** frame);
void engine_process_frame_to_ascii(ProcessingContext* ctx, const struct AVFrame* frame, const EngineConfig* config);

void engine_render_to_console(ProcessingContext* ctx, const EngineConfig* config);
int engine_render_to_image_file(ProcessingContext* ctx, const EngineConfig* config);

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

