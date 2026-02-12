/*
 * =====================================================================================
 * engine_internal.h - Internal shared header for engine implementations
 * Exposes ProcessingContext and ThreadArgs for engine variants (haruhi, bvoid, etc.)
 * =====================================================================================
 */

#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H

#include <stdint.h>
#include <pthread.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "ascii_engine.h"

/* Memory Arena for per-frame allocation */
typedef struct {
    void* start;
    size_t size;
    size_t used;
} Arena;

/* Forward declaration */
struct ProcessingContext;

/* Thread work arguments */
typedef struct {
    struct ProcessingContext* ctx;
    const EngineConfig* config;
    const AVFrame* frame;
    int start_row;
    int end_row;
} ThreadArgs;

/* Main processing context - shared across all engine variants */
struct ProcessingContext {
    AVFormatContext* dec_fmt_ctx;
    const AVCodec* dec_codec;
    AVCodecContext* dec_codec_ctx;
    int video_stream_idx;
    int audio_stream_idx;
    AVRational time_base;

    AVFormatContext* enc_fmt_ctx;
    AVStream* out_video_stream;
    AVStream* out_audio_stream;
    const AVCodec* enc_codec;
    AVCodecContext* enc_codec_ctx;

    AVFrame *decoded_frame;
    AVFrame *rgb_frame;
    AVFrame *yuv_frame;

    struct SwsContext* sws_ctx_to_rgb;
    struct SwsContext* sws_ctx_to_yuv;

    char* char_buffer;
    unsigned char* color_buffer;
    int ascii_width;
    int ascii_height;

    Arena frame_arena;
    pthread_t* workers;
    ThreadArgs* worker_args;
    int num_threads;
    
    /* LUTs for character selection */
    char char_lut_flat[256];
    char char_lut_vert[256];
    char char_lut_horz[256];
    char char_lut_diag1[256];
    char char_lut_diag2[256];
    uint8_t gamma_lut[256];
};

/* Alias for convenience */
typedef struct ProcessingContext ProcessingContext;

#endif /* ENGINE_INTERNAL_H */
