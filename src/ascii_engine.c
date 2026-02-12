/*
 * =====================================================================================
 *
 * Filename:  ascii_engine.c
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
#include <libavutil/rational.h> // For av_q2d

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "font8x8_basic.h"
#include "ascii_engine.h"
#include "simd_ops.h"

// ═══════════════════════════════════════════════════════════════════════════
//                        BVOID RAMP SYSTEM (GOD_TIER)
// ═══════════════════════════════════════════════════════════════════════════
#define BVOID_RAMP_DEFAULT  0
#define BVOID_RAMP_BLOCKS   1
#define BVOID_RAMP_UNICODE  5
#define BVOID_RAMP_BRAILLE  6

// Built-in character ramps
static const char* BVOID_RAMP_STANDARD = "`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
static const char* BVOID_RAMP_BLOCKS_STR = " ░▒▓█";
static const char* BVOID_RAMP_UNICODE_STR = " ·∙•○◐●◉█";  // Extended unicode gradient

// Current ramp settings (global for BVOID engine)
static int g_bvoid_ramp_mode = BVOID_RAMP_DEFAULT;
static const char* g_bvoid_current_ramp = NULL;
static int g_bvoid_ramp_len = 0;

// Initialize BVOID ramp based on mode
static void bvoid_init_ramp(int mode) {
    g_bvoid_ramp_mode = mode;
    switch (mode) {
        case BVOID_RAMP_BLOCKS:
            g_bvoid_current_ramp = BVOID_RAMP_BLOCKS_STR;
            g_bvoid_ramp_len = 5;  // Unicode chars are multi-byte!
            break;
        case BVOID_RAMP_UNICODE:
            g_bvoid_current_ramp = BVOID_RAMP_UNICODE_STR;
            g_bvoid_ramp_len = 9;
            break;
        default:
            g_bvoid_current_ramp = BVOID_RAMP_STANDARD;
            g_bvoid_ramp_len = 90;
            break;
    }
}

// Get character from BVOID unicode ramp (handles UTF-8)
static const char* bvoid_get_unicode_char(float brightness) {
    // Unicode character lookup for blocks mode
    static const char* UNICODE_SHADING[] = {" ", "░", "▒", "▓", "█"};
    static const char* UNICODE_GRADIENT[] = {" ", "·", "∙", "•", "○", "◐", "●", "◉", "█"};
    
    if (g_bvoid_ramp_mode == BVOID_RAMP_BLOCKS) {
        int idx = (int)(brightness * 4 / 255);
        if (idx < 0) idx = 0;
        if (idx > 4) idx = 4;
        return UNICODE_SHADING[idx];
    } else if (g_bvoid_ramp_mode == BVOID_RAMP_UNICODE) {
        int idx = (int)(brightness * 8 / 255);
        if (idx < 0) idx = 0;
        if (idx > 8) idx = 8;
        return UNICODE_GRADIENT[idx];
    }
    return " ";
}



// --- Memory Arena ---
// Why an arena? Standard malloc/free incurs overhead and can fragment memory. For
// per-frame processing, where we allocate and discard numerous buffers, an arena
// is king. We allocate a large block once, then simply bump a pointer for each
// allocation. A reset is a trivial pointer move. This is a data-oriented approach
// that respects the cache and minimizes system call overhead.
typedef struct {
    void* start;
    size_t size;
    size_t used;
} Arena;

static int arena_init(Arena* a, size_t size) {
    a->start = malloc(size);
    if (!a->start) return 0;
    a->size = size;
    a->used = 0;
    return 1;
}

static void* arena_alloc(Arena* a, size_t size) {
    // Why align? SIMD operations and many other CPU-level instructions perform
    // better on aligned data. Aligning to 16 bytes is a safe bet for SSE and
    // general performance. The bitwise trick is faster than division/modulo.
    size = (size + 15) & ~15;
    if (a->used + size > a->size) return NULL;
    void* p = (char*)a->start + a->used;
    a->used += size;
    return p;
}

static void arena_reset(Arena* a) {
    a->used = 0;
}

static void arena_free(Arena* a) {
    free(a->start);
    a->start = NULL;
    a->size = 0;
    a->used = 0;
}

// --- Threading & Work ---
typedef struct {
    ProcessingContext* ctx;
    const EngineConfig* config;
    const AVFrame* frame;
    int start_row;
    int end_row;
} ThreadArgs;


struct ProcessingContext {
    AVFormatContext* dec_fmt_ctx;
    const AVCodec* dec_codec;
    AVCodecContext* dec_codec_ctx;
    int video_stream_idx;
    int audio_stream_idx;
    AVRational time_base; // Why AVRational? Frame PTS are in terms of this time_base. Storing it is essential for correct timing calculations.

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
    
    // Why multiple LUTs for characters? Edge detection is not just about magnitude,
    // but direction. By pre-calculating ramps for different edge orientations
    // (vertical, horizontal, diagonal), we can select a character that visually
    // represents the edge's flow, leading to a much more structured and detailed
    // final image. This is a classic demoscene trick.
    char char_lut_flat[256];
    char char_lut_vert[256];
    char char_lut_horz[256];
    char char_lut_diag1[256];
    char char_lut_diag2[256];

    // Why a gamma LUT? The powf() function is computationally expensive. For a
    // fixed gamma correction (2.2), the result for each of the 256 possible
    // byte values is constant. Pre-calculating these into a lookup table (LUT)
    // transforms an expensive floating-point power calculation into a single,
    // lightning-fast array lookup per pixel component.
    uint8_t gamma_lut[256];
};


static const char* init_encoder(ProcessingContext* ctx, const EngineConfig* config);
static void* process_slice_worker(void* arg);

static void init_luts(ProcessingContext* ctx) {
    // ═══════════════════════════════════════════════════════════════════════════
    //                BVOID GOD_TIER RAMP SYSTEM - DYNAMIC LUT INIT
    // ═══════════════════════════════════════════════════════════════════════════
    
    const char* flat_ramp;
    int flat_ramp_len;
    
    // Select ramp based on global mode (set by bvoid_init_ramp)
    // *** MATCHED TO HARUHI ENGINE FOR CONSISTENCY ***
    switch (g_bvoid_ramp_mode) {
        case BVOID_RAMP_BLOCKS:
            // Match HARUHI RAMP_MODE_BLOCKS: " .oO@" approximation
            // HARUHI uses " ░▒▓█" but falls back to ASCII for bitmap render
            flat_ramp = " .oO@";  // 5 chars - exact match to HARUHI
            flat_ramp_len = 5;
            break;
            
        case BVOID_RAMP_UNICODE:
            // Match HARUHI RAMP_MODE_UNICODE unicode_fallback[]
            // HARUHI line 913: " .oO@"
            flat_ramp = " .oO@";  // 5 chars - exact match to HARUHI
            flat_ramp_len = 5;
            break;
            
        case BVOID_RAMP_BRAILLE:
            // Match HARUHI RAMP_MODE_BRAILLE braille_density[]
            // HARUHI line 922: " .':-=+*#"
            flat_ramp = " .':-=+*#";  // 9 chars - exact match to HARUHI
            flat_ramp_len = 9;
            break;
            
        default: // BVOID_RAMP_DEFAULT
            // Full 90-character gradient for maximum detail
            flat_ramp = "`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
            flat_ramp_len = 90;
            break;
    }
    
    // Edge direction characters
    const char* vertical_ramp = "|I"; const int vertical_ramp_len = 2;
    const char* horizontal_ramp = "-="; const int horizontal_ramp_len = 2;
    const char* diagonal1_ramp = "\\_"; const int diagonal1_ramp_len = 2;
    const char* diagonal2_ramp = "/_"; const int diagonal2_ramp_len = 2;

    for (int i = 0; i < 256; i++) {
        float br = (float)i / 255.0f;
        ctx->char_lut_flat[i] = flat_ramp[(int)(br * (flat_ramp_len - 1))];
        ctx->char_lut_vert[i] = vertical_ramp[(int)(br * (vertical_ramp_len - 1))];
        ctx->char_lut_horz[i] = horizontal_ramp[(int)(br * (horizontal_ramp_len - 1))];
        ctx->char_lut_diag1[i] = diagonal1_ramp[(int)(br * (diagonal1_ramp_len - 1))];
        ctx->char_lut_diag2[i] = diagonal2_ramp[(int)(br * (diagonal2_ramp_len - 1))];

        // Gamma LUT for brightness correction
        ctx->gamma_lut[i] = (uint8_t)(powf((float)i / 255.0f, 1.0f / 2.2f) * 255.0f);
    }
}


int is_animated_file(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return 0;
    // Why treat GIF as a video? Because it is. It's a container of frames with
    // timing information, just like MP4. FFmpeg's libavformat handles this
    // abstraction beautifully, allowing us to use the same decoding pipeline.
    return (strcmp(ext, ".mp4") == 0 || strcmp(ext, ".mkv") == 0 ||
            strcmp(ext, ".avi") == 0 || strcmp(ext, ".mov") == 0 ||
            strcmp(ext, ".gif") == 0);
}

ProcessingContext* engine_init(const char* input_source, const EngineConfig* config, char** error) {
    ProcessingContext* ctx = (ProcessingContext*)calloc(1, sizeof(ProcessingContext));
    if (!ctx) { *error = "Failed to allocate context"; return NULL; }

    ctx->video_stream_idx = -1;
    ctx->audio_stream_idx = -1;
    
    // Initialize BVOID ramp system based on config
    bvoid_init_ramp(config->ramp_mode);
    init_luts(ctx);

    if (config->num_threads > 0) {
        ctx->num_threads = config->num_threads;
    } else {
        ctx->num_threads = (int)sysconf(_SC_NPROCESSORS_ONLN);
        if (ctx->num_threads <= 0) ctx->num_threads = 1;
    }
    ctx->workers = (pthread_t*)malloc(ctx->num_threads * sizeof(pthread_t));
    ctx->worker_args = (ThreadArgs*)malloc(ctx->num_threads * sizeof(ThreadArgs));
    if (!ctx->workers || !ctx->worker_args) {
        *error = "Failed to allocate threading resources";
        engine_cleanup(&ctx);
        return NULL;
    }

    if (config->mode == MODE_VIDEO || config->mode == MODE_ANIMATED_GIF) {
        if (avformat_open_input(&ctx->dec_fmt_ctx, input_source, NULL, NULL) != 0) {
            *error = "Couldn't open video file"; engine_cleanup(&ctx); return NULL;
        }
        if (avformat_find_stream_info(ctx->dec_fmt_ctx, NULL) < 0) {
            *error = "Couldn't find stream information"; engine_cleanup(&ctx); return NULL;
        }
        for (unsigned int i = 0; i < ctx->dec_fmt_ctx->nb_streams; i++) {
            if (ctx->dec_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && ctx->video_stream_idx < 0) {
                ctx->video_stream_idx = i;
            }
            if (ctx->dec_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && ctx->audio_stream_idx < 0) {
                ctx->audio_stream_idx = i;
            }
        }
        if (ctx->video_stream_idx == -1) { *error = "Didn't find a video stream"; engine_cleanup(&ctx); return NULL; }

        AVStream* video_stream = ctx->dec_fmt_ctx->streams[ctx->video_stream_idx];
        AVCodecParameters* pCodecPar = video_stream->codecpar;
        ctx->dec_codec = avcodec_find_decoder(pCodecPar->codec_id);
        if (!ctx->dec_codec) { *error = "Unsupported decoder"; engine_cleanup(&ctx); return NULL; }
        ctx->dec_codec_ctx = avcodec_alloc_context3(ctx->dec_codec);
        if (!ctx->dec_codec_ctx) { *error = "Failed to alloc decoder context"; engine_cleanup(&ctx); return NULL; }
        if (avcodec_parameters_to_context(ctx->dec_codec_ctx, pCodecPar) < 0) { *error = "Couldn't copy decoder context"; engine_cleanup(&ctx); return NULL; }
        if (avcodec_open2(ctx->dec_codec_ctx, ctx->dec_codec, NULL) < 0) {
            *error = "Could not open decoder codec"; engine_cleanup(&ctx); return NULL;
        }
        ctx->time_base = video_stream->time_base;
    } else { // MODE_IMAGE
        int width, height, channels;
        unsigned char* data = stbi_load(input_source, &width, &height, &channels, 3);
        if (!data) { *error = (char*)stbi_failure_reason(); engine_cleanup(&ctx); return NULL; }

        ctx->dec_codec_ctx = avcodec_alloc_context3(NULL);
        if (!ctx->dec_codec_ctx) { *error = "Failed to alloc image context"; stbi_image_free(data); engine_cleanup(&ctx); return NULL; }
        ctx->dec_codec_ctx->width = width;
        ctx->dec_codec_ctx->height = height;
        ctx->dec_codec_ctx->pix_fmt = AV_PIX_FMT_RGB24;

        ctx->decoded_frame = av_frame_alloc();
        if (!ctx->decoded_frame) { *error = "Failed to alloc image frame"; stbi_image_free(data); engine_cleanup(&ctx); return NULL; }
        ctx->decoded_frame->width = width;
        ctx->decoded_frame->height = height;
        ctx->decoded_frame->format = AV_PIX_FMT_RGB24;
        av_image_alloc(ctx->decoded_frame->data, ctx->decoded_frame->linesize, width, height, AV_PIX_FMT_RGB24, 1);
        memcpy(ctx->decoded_frame->data[0], data, (size_t)width * height * 3);
        stbi_image_free(data);
    }

    ctx->ascii_width = config->output_width;
    ctx->ascii_height = (int)((float)ctx->ascii_width / ((float)ctx->dec_codec_ctx->width / ctx->dec_codec_ctx->height) * config->aspect_correction);


    ctx->sws_ctx_to_rgb = sws_getContext(ctx->dec_codec_ctx->width, ctx->dec_codec_ctx->height, ctx->dec_codec_ctx->pix_fmt,
                                         ctx->dec_codec_ctx->width, ctx->dec_codec_ctx->height, AV_PIX_FMT_RGB24,
                                         SWS_BILINEAR, NULL, NULL, NULL);

    if (config->mode != MODE_IMAGE) ctx->decoded_frame = av_frame_alloc();
    ctx->rgb_frame = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, ctx->dec_codec_ctx->width, ctx->dec_codec_ctx->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) { *error = "Failed to alloc RGB buffer"; engine_cleanup(&ctx); return NULL; }
    av_image_fill_arrays(ctx->rgb_frame->data, ctx->rgb_frame->linesize, buffer, AV_PIX_FMT_RGB24, ctx->dec_codec_ctx->width, ctx->dec_codec_ctx->height, 1);

    if (!arena_init(&ctx->frame_arena, 64 * 1024 * 1024)) { // Increased arena size for larger resolutions
        *error = "Failed to initialize memory arena";
        engine_cleanup(&ctx);
        return NULL;
    }

    if (config->output_filename && is_animated_file(config->output_filename)) {
        const char* encoder_error = init_encoder(ctx, config);
        if (encoder_error) {
            *error = (char*)encoder_error;
            engine_cleanup(&ctx);
            return NULL;
        }
    }

    return ctx;
}

void engine_cleanup(ProcessingContext** ctx_ptr) {
    if (!ctx_ptr || !*ctx_ptr) return;
    ProcessingContext* ctx = *ctx_ptr;

    arena_free(&ctx->frame_arena);
    free(ctx->workers);
    free(ctx->worker_args);

    if (ctx->rgb_frame) { av_freep(&ctx->rgb_frame->data[0]); av_frame_free(&ctx->rgb_frame); }
    if (ctx->yuv_frame) { av_freep(&ctx->yuv_frame->data[0]); av_frame_free(&ctx->yuv_frame); }
    if (ctx->decoded_frame) av_frame_free(&ctx->decoded_frame);

    if (ctx->dec_codec_ctx) avcodec_free_context(&ctx->dec_codec_ctx);
    if (ctx->dec_fmt_ctx) avformat_close_input(&ctx->dec_fmt_ctx);

    if (ctx->enc_codec_ctx) avcodec_free_context(&ctx->enc_codec_ctx);
    if (ctx->enc_fmt_ctx) {
        if (!(ctx->enc_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&ctx->enc_fmt_ctx->pb);
        }
        avformat_free_context(ctx->enc_fmt_ctx);
    }

    if (ctx->sws_ctx_to_rgb) sws_freeContext(ctx->sws_ctx_to_rgb);
    if (ctx->sws_ctx_to_yuv) sws_freeContext(ctx->sws_ctx_to_yuv);

    free(ctx);
    *ctx_ptr = NULL;
}

int engine_get_next_packet(ProcessingContext* ctx, AVPacket* packet) {
    if (!ctx || !ctx->dec_fmt_ctx) return AVERROR_EOF;
    return av_read_frame(ctx->dec_fmt_ctx, packet);
}

int engine_decode_video_packet(ProcessingContext* ctx, AVPacket* packet, struct AVFrame** frame) {
    if (!ctx) return AVERROR_INVALIDDATA;
    
    if (ctx->dec_fmt_ctx == NULL) {
        if (ctx->decoded_frame) {
            *frame = ctx->decoded_frame;
            ctx->decoded_frame = NULL;
            return 0;
        }
        return AVERROR_EOF;
    }

    int ret = avcodec_send_packet(ctx->dec_codec_ctx, packet);
    if (ret < 0) return ret;
    ret = avcodec_receive_frame(ctx->dec_codec_ctx, ctx->decoded_frame);
    if (ret == 0) *frame = ctx->decoded_frame;
    return ret;
}

double engine_get_frame_delay_secs(const ProcessingContext* ctx, const AVFrame* frame) {
    if (!ctx || !frame || !ctx->dec_fmt_ctx) return 1.0 / 24.0; // Default fallback
    
    AVStream* stream = ctx->dec_fmt_ctx->streams[ctx->video_stream_idx];
    
    // Why this logic? Different formats provide timing differently. A fixed frame rate
    // is an approximation. The Presentation Timestamp (PTS) is the ground truth.
    // We convert the PTS from its stream-specific time base into seconds. For GIFs,
    // which store duration in metadata, we prioritize that. This ensures the most
    // accurate playback timing for any given format.
    if (frame->duration > 0) {
        return frame->duration * av_q2d(stream->time_base);
    }
    
    if (stream->avg_frame_rate.den > 0) {
        return 1.0 / av_q2d(stream->avg_frame_rate);
    }

    return 1.0 / 24.0; // Final fallback
}


void engine_process_frame_to_ascii(ProcessingContext* ctx, const struct AVFrame* frame, const EngineConfig* config) {
    arena_reset(&ctx->frame_arena);
    
    size_t char_buffer_size = (size_t)(ctx->ascii_width) * ctx->ascii_height;
    ctx->char_buffer = (char*)arena_alloc(&ctx->frame_arena, char_buffer_size * sizeof(char));
    ctx->color_buffer = (unsigned char*)arena_alloc(&ctx->frame_arena, char_buffer_size * 3 * sizeof(unsigned char));

    if (!ctx->char_buffer || !ctx->color_buffer) {
        fprintf(stderr, "Arena allocation failed for frame buffers.\n");
        return;
    }

    sws_scale(ctx->sws_ctx_to_rgb, (uint8_t const * const *)frame->data,
              frame->linesize, 0, frame->height,
              ctx->rgb_frame->data, ctx->rgb_frame->linesize);

    // V17: Route to HARUHI engine if enabled
    if (config->use_haruhi) {
        extern void engine_process_haruhi(ProcessingContext*, const struct AVFrame*, const EngineConfig*);
        engine_process_haruhi(ctx, frame, config);
        return;
    }

    // Base engine processing
    int rows_per_thread = ctx->ascii_height / ctx->num_threads;
    for (int i = 0; i < ctx->num_threads; ++i) {
        ThreadArgs* args = &ctx->worker_args[i];
        args->ctx = ctx;
        args->config = config;
        args->frame = frame;
        args->start_row = i * rows_per_thread;
        args->end_row = (i == ctx->num_threads - 1) ? ctx->ascii_height : (i + 1) * rows_per_thread;

        pthread_create(&ctx->workers[i], NULL, process_slice_worker, args);
    }

    for (int i = 0; i < ctx->num_threads; ++i) {
        pthread_join(ctx->workers[i], NULL);
    }
}

static void* process_slice_worker(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    ProcessingContext* ctx = args->ctx;
    const EngineConfig* config = args->config;
    
    int width = ctx->dec_codec_ctx->width;
    int height = ctx->dec_codec_ctx->height;
    uint8_t* data = ctx->rgb_frame->data[0];
    int stride = ctx->rgb_frame->linesize[0];

    const float edge_strength_sq = config->edge_strength * config->edge_strength;

    for (int y = args->start_row; y < args->end_row; y++) {
        for (int x = 0; x < ctx->ascii_width; x++) {
            int source_x = (int)((float)x / ctx->ascii_width * width);
            int source_y = (int)((float)y / ctx->ascii_height * height);

            float gx = 0.0f, gy = 0.0f;
            float center_luma = 0.0f;

            // Why Sobel? It's a fundamental, efficient way to calculate the image
            // gradient. By sampling a 3x3 grid, we approximate the derivative in
            // both X and Y directions, giving us the information needed to detect
            // edges and their orientation. It's a classic for a reason.
            const int sobel_y[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
            const int sobel_x[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int sx = source_x + kx;
                    int sy = source_y + ky;
                    sx = (sx < 0) ? 0 : (sx >= width ? width - 1 : sx);
                    sy = (sy < 0) ? 0 : (sy >= height ? height - 1 : sy);

                    uint8_t* p = data + (sy * stride + sx * 3);
                    float luma = (0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2]);

                    gx += luma * sobel_x[ky + 1][kx + 1];
                    gy += luma * sobel_y[ky + 1][kx + 1];

                    if (kx == 0 && ky == 0) {
                        center_luma = luma;
                    }
                }
            }
            
            float mag_sq = (gx * gx + gy * gy) / (255.0f * 255.0f);
            
            // This is the optimization. Instead of a costly powf() call for every
            // pixel, we use a single, fast lookup into our pre-calculated table.
            uint8_t brightness_idx = ctx->gamma_lut[(uint8_t)center_luma];
            
            // V20: Apply dithering to brightness
            if (config->dither_type > 0) {
                // Bayer 4x4 dithering matrix
                static const int bayer4x4[4][4] = {
                    { 0,  8,  2, 10},
                    {12,  4, 14,  6},
                    { 3, 11,  1,  9},
                    {15,  7, 13,  5}
                };
                
                int dither_offset = 0;
                switch (config->dither_type) {
                    case 1: // Floyd-Steinberg style (simple random spread)
                        dither_offset = ((x * 17 + y * 31) % 32) - 16;
                        break;
                    case 2: // Bayer ordered dithering
                        dither_offset = (bayer4x4[y % 4][x % 4] - 8) * 4;
                        break;
                    case 3: // Ordered 8x8 style
                        dither_offset = (bayer4x4[y % 4][x % 4] - 8) * 8;
                        break;
                }
                
                int new_brightness = brightness_idx + dither_offset;
                brightness_idx = (new_brightness < 0) ? 0 : (new_brightness > 255 ? 255 : new_brightness);
            }

            char selected_char;
            
            // Dark threshold: Use space for very dark areas to reduce noise
            if (brightness_idx < 10) {
                selected_char = ' ';
            } else if (mag_sq < edge_strength_sq) {
                selected_char = ctx->char_lut_flat[brightness_idx];
            } else {
                // Why this logic? The ratio of gx to gy tells us the angle of the
                // gradient. A large gy/gx ratio means a near-vertical edge. A large
                // gx/gy ratio means a near-horizontal one. The sign of gx*gy tells
                // us the diagonal direction. This allows us to select a character
                // that visually matches the edge's orientation.
                const float D_THRESH = 2.41421356f; // tan(67.5 degrees)
                if (fabsf(gy) > fabsf(gx) * D_THRESH) {
                    selected_char = ctx->char_lut_vert[brightness_idx];
                } else if (fabsf(gx) > fabsf(gy) * D_THRESH) {
                    selected_char = ctx->char_lut_horz[brightness_idx];
                } else {
                    selected_char = (gx * gy > 0) ? ctx->char_lut_diag1[brightness_idx]
                                                  : ctx->char_lut_diag2[brightness_idx];
                }
            }

            int art_idx = y * ctx->ascii_width + x;
            ctx->char_buffer[art_idx] = selected_char;
            uint8_t* p_color = data + (source_y * stride + source_x * 3);
            ctx->color_buffer[art_idx * 3 + 0] = p_color[0];
            ctx->color_buffer[art_idx * 3 + 1] = p_color[1];
            ctx->color_buffer[art_idx * 3 + 2] = p_color[2];
        }
    }
    return NULL;
}


void engine_render_to_console(ProcessingContext* ctx, const EngineConfig* config) {
    // Why use an arena-allocated buffer? Building the entire frame string in memory
    // before printing avoids thousands of tiny printf calls, which would cause
    // flickering and be incredibly slow. We do one single, massive write to stdout,
    // ensuring the frame appears atomically.
    size_t full_buffer_size = (size_t)(ctx->ascii_width * 20 + 2) * ctx->ascii_height;
    char* full_frame_buffer = (char*)arena_alloc(&ctx->frame_arena, full_buffer_size);
    if (!full_frame_buffer) return;

    char* buf_ptr = full_frame_buffer;
    for (int y = 0; y < ctx->ascii_height; y++) {
        for (int x = 0; x < ctx->ascii_width; x++) {
            int idx = y * ctx->ascii_width + x;
            if (config->use_color) {
                unsigned char r = ctx->color_buffer[idx * 3 + 0];
                unsigned char g = ctx->color_buffer[idx * 3 + 1];
                unsigned char b = ctx->color_buffer[idx * 3 + 2];
                buf_ptr += sprintf(buf_ptr, "\x1b[38;2;%d;%d;%dm%c", r, g, b, ctx->char_buffer[idx]);
            } else {
                *buf_ptr++ = ctx->char_buffer[idx];
            }
        }
        *buf_ptr++ = '\n';
    }
    *buf_ptr = '\0';
    // Why \x1b[H? This is an ANSI escape code that moves the cursor to the home
    // position (top-left). This allows us to overwrite the previous frame in-place
    // in the terminal, creating a smooth animation instead of a scrolling mess.
    printf("\x1b[H%s", full_frame_buffer);
    fflush(stdout);
}

static void render_ascii_to_buffer(ProcessingContext* ctx, unsigned char* buffer, const EngineConfig* config) {
    int out_img_width = ctx->ascii_width * 8;
    int out_img_height = ctx->ascii_height * 8;
    memset(buffer, 0, (size_t)out_img_width * out_img_height * 3);

    for (int y = 0; y < ctx->ascii_height; y++) {
        for (int x = 0; x < ctx->ascii_width; x++) {
            int art_idx = y * ctx->ascii_width + x;
            unsigned char char_code = (unsigned char)ctx->char_buffer[art_idx];
            unsigned char* glyph = (unsigned char*)font8x8_basic[char_code];

            unsigned char r_in = ctx->color_buffer[art_idx * 3 + 0];
            unsigned char g_in = ctx->color_buffer[art_idx * 3 + 1];
            unsigned char b_in = ctx->color_buffer[art_idx * 3 + 2];
            
            unsigned char r, g, b;
            
            // V20: Pure Mode - skip ALL color processing
            if (config->pure_mode) {
                r = r_in;
                g = g_in;
                b = b_in;
            } else {
                // Normal color processing path
                if (config->saturation_factor != 1.0f) {
                    float luma = (0.299f * r_in + 0.587f * g_in + 0.114f * b_in);
                    float r_f = luma + config->saturation_factor * (r_in - luma);
                    float g_f = luma + config->saturation_factor * (g_in - luma);
                    float b_f = luma + config->saturation_factor * (b_in - luma);
                    r_in = (r_f > 255.0f) ? 255 : ((r_f < 0) ? 0 : (unsigned char)r_f);
                    g_in = (g_f > 255.0f) ? 255 : ((g_f < 0) ? 0 : (unsigned char)g_f);
                    b_in = (b_f > 255.0f) ? 255 : ((b_f < 0) ? 0 : (unsigned char)b_f);
                }

                float r_f = r_in * config->brightness_factor;
                float g_f = g_in * config->brightness_factor;
                float b_f = b_in * config->brightness_factor;

                if (config->use_hdr) {
                    r_f = (r_f / (r_f + 128.0f)) * 255.0f * 2.0f;
                    g_f = (g_f / (g_f + 128.0f)) * 255.0f * 2.0f;
                    b_f = (b_f / (b_f + 128.0f)) * 255.0f * 2.0f;
                }

                if (config->color_grade > 0) {
                    float luma = 0.299f * r_f + 0.587f * g_f + 0.114f * b_f;
                    switch (config->color_grade) {
                        case 1: // Cinematic
                            if (luma < 128) { r_f *= 0.85f; g_f *= 1.05f; b_f *= 1.15f; }
                            else { r_f *= 1.15f; b_f *= 0.85f; }
                            r_f = 128 + (r_f - 128) * 1.2f;
                            g_f = 128 + (g_f - 128) * 1.2f;
                            b_f = 128 + (b_f - 128) * 1.2f;
                            break;
                        case 2: // Noir
                            r_f = luma + (r_f - luma) * 0.3f;
                            g_f = luma + (g_f - luma) * 0.3f;
                            b_f = luma + (b_f - luma) * 0.3f;
                            r_f = 128 + (r_f - 128) * 1.5f;
                            g_f = 128 + (g_f - 128) * 1.5f;
                            b_f = 128 + (b_f - 128) * 1.5f;
                            b_f *= 1.1f;
                            break;
                        case 3: // Vintage
                            r_f = luma + (r_f - luma) * 0.6f;
                            g_f = luma + (g_f - luma) * 0.6f;
                            b_f = luma + (b_f - luma) * 0.6f;
                            r_f = r_f * 1.2f + 15; g_f = g_f * 1.0f + 10; b_f = b_f * 0.8f - 5;
                            r_f = r_f * 0.9f + 20; g_f = g_f * 0.9f + 18; b_f = b_f * 0.9f + 15;
                            break;
                    }
                }

                r = (r_f > 255.0f) ? 255 : ((r_f < 0) ? 0 : (unsigned char)r_f);
                g = (g_f > 255.0f) ? 255 : ((g_f < 0) ? 0 : (unsigned char)g_f);
                b = (b_f > 255.0f) ? 255 : ((b_f < 0) ? 0 : (unsigned char)b_f);
            }
            
            // V20: Edge-Only Mode - only render if character is not space
            if (config->edge_only && char_code == ' ') {
                continue;  // Skip non-edge areas
            }
            
            // V20: Pixelate Mode - fill background with solid color first
            if (config->pixelate_size > 0) {
                for (int gy = 0; gy < 8; gy++) {
                    for (int gx = 0; gx < 8; gx++) {
                        int img_x = x * 8 + gx;
                        int img_y = y * 8 + gy;
                        int img_idx = (img_y * out_img_width + img_x) * 3;
                        // Fill with slightly darker version as background
                        buffer[img_idx + 0] = (unsigned char)(r * 0.6f);
                        buffer[img_idx + 1] = (unsigned char)(g * 0.6f);
                        buffer[img_idx + 2] = (unsigned char)(b * 0.6f);
                    }
                }
            }

            // Render character glyph
            for (int gy = 0; gy < 8; gy++) {
                for (int gx = 0; gx < 8; gx++) {
                    if ((glyph[gy] >> (7 - gx)) & 1) {
                        int img_x = x * 8 + gx;
                        int img_y = y * 8 + gy;
                        int img_idx = (img_y * out_img_width + img_x) * 3;
                        buffer[img_idx + 0] = r;
                        buffer[img_idx + 1] = g;
                        buffer[img_idx + 2] = b;
                    }
                }
            }
        }
    }
}


int engine_render_to_image_file(ProcessingContext* ctx, const EngineConfig* config) {
    int out_img_width = ctx->ascii_width * 8;
    int out_img_height = ctx->ascii_height * 8;
    unsigned char* out_img_data = (unsigned char*)calloc((size_t)out_img_width * out_img_height * 3, 1);
    if (!out_img_data) { return -1; }

    render_ascii_to_buffer(ctx, out_img_data, config);

    // V17: Glow/Bloom post-processing effect
    if (config->glow_strength > 0.0f) {
        int total_pixels = out_img_width * out_img_height;
        unsigned char* glow_buffer = (unsigned char*)calloc((size_t)total_pixels * 3, 1);
        if (glow_buffer) {
            // Pass 1: Extract bright pixels and spread glow
            const int glow_radius = 4;
            for (int y = glow_radius; y < out_img_height - glow_radius; y++) {
                for (int x = glow_radius; x < out_img_width - glow_radius; x++) {
                    int idx = (y * out_img_width + x) * 3;
                    int luma = (out_img_data[idx] * 299 + out_img_data[idx+1] * 587 + out_img_data[idx+2] * 114) / 1000;
                    
                    // Only glow bright pixels (threshold 180)
                    if (luma > 180) {
                        float intensity = (luma - 180) / 75.0f * config->glow_strength;
                        if (intensity > 1.0f) intensity = 1.0f;
                        
                        // Spread glow to nearby pixels
                        for (int dy = -glow_radius; dy <= glow_radius; dy++) {
                            for (int dx = -glow_radius; dx <= glow_radius; dx++) {
                                float dist = sqrtf((float)(dx*dx + dy*dy)) / glow_radius;
                                if (dist <= 1.0f) {
                                    float falloff = (1.0f - dist) * intensity * 0.3f;
                                    int gidx = ((y + dy) * out_img_width + (x + dx)) * 3;
                                    glow_buffer[gidx + 0] = (unsigned char)fminf(255.0f, glow_buffer[gidx + 0] + out_img_data[idx + 0] * falloff);
                                    glow_buffer[gidx + 1] = (unsigned char)fminf(255.0f, glow_buffer[gidx + 1] + out_img_data[idx + 1] * falloff);
                                    glow_buffer[gidx + 2] = (unsigned char)fminf(255.0f, glow_buffer[gidx + 2] + out_img_data[idx + 2] * falloff);
                                }
                            }
                        }
                    }
                }
            }
            
            // Pass 2: Blend glow into original
            for (int i = 0; i < total_pixels * 3; i++) {
                int blended = out_img_data[i] + glow_buffer[i];
                out_img_data[i] = (blended > 255) ? 255 : (unsigned char)blended;
            }
            free(glow_buffer);
        }
    }

    // V18: CRT Scanline effect (improved for better visual quality)
    if (config->scanline_strength > 0.0f || config->crt_strength > 0.0f) {
        // Brightness compensation for the darkening effects
        float brightness_comp = 1.0f;
        if (config->scanline_strength > 0.0f) brightness_comp += config->scanline_strength * 0.15f;
        if (config->crt_strength > 0.0f) brightness_comp += config->crt_strength * 0.1f;
        
        for (int y = 0; y < out_img_height; y++) {
            // Scanlines: Darken every 4th row for subtle effect
            float scanline_factor = 1.0f;
            if (config->scanline_strength > 0.0f && (y % 4 == 3)) {
                scanline_factor = 1.0f - config->scanline_strength * 0.3f;
            }
            
            for (int x = 0; x < out_img_width; x++) {
                int idx = (y * out_img_width + x) * 3;
                
                float r = out_img_data[idx + 0] * brightness_comp;
                float g = out_img_data[idx + 1] * brightness_comp;
                float b = out_img_data[idx + 2] * brightness_comp;
                
                // Apply scanline darkening
                r *= scanline_factor;
                g *= scanline_factor;
                b *= scanline_factor;
                
                // CRT phosphor effect: subtle highlight on corresponding subpixel
                // Only apply to non-black pixels to avoid noise
                if (config->crt_strength > 0.0f && (r + g + b) > 30) {
                    int sub_x = x % 3;
                    float boost = 1.0f + config->crt_strength * 0.08f;  // Much subtler
                    
                    // Subtle color channel emphasis (not full separation)
                    if (sub_x == 0) {       // Slight R boost
                        r *= boost;
                    } else if (sub_x == 1) { // Slight G boost
                        g *= boost;
                    } else {                 // Slight B boost
                        b *= boost;
                    }
                }
                
                out_img_data[idx + 0] = (r > 255) ? 255 : (unsigned char)r;
                out_img_data[idx + 1] = (g > 255) ? 255 : (unsigned char)g;
                out_img_data[idx + 2] = (b > 255) ? 255 : (unsigned char)b;
            }
        }
    }

    if (stbi_write_png(config->output_filename, out_img_width, out_img_height, 3, out_img_data, out_img_width * 3) == 0) {
        free(out_img_data);
        return -1;
    }
    
    free(out_img_data);
    return 0;
}

// =====================================================================================
// SVG Export Engine - Resolution Independent Vector Output
// Each character is a scalable text element that maintains quality at any zoom level
// =====================================================================================

static void svg_escape_char(char c, char* out) {
    switch (c) {
        case '<':  strcpy(out, "&lt;"); break;
        case '>':  strcpy(out, "&gt;"); break;
        case '&':  strcpy(out, "&amp;"); break;
        case '"':  strcpy(out, "&quot;"); break;
        case '\'': strcpy(out, "&apos;"); break;
        default:   out[0] = c; out[1] = '\0'; break;
    }
}

int engine_render_to_svg_file(ProcessingContext* ctx, const EngineConfig* config) {
    FILE* fp = fopen(config->output_filename, "w");
    if (!fp) { return -1; }
    
    // Match the PNG renderer's 8x8 cell size for identical proportions
    const int char_width = 8;
    const int char_height = 8;
    int svg_width = ctx->ascii_width * char_width;
    int svg_height = ctx->ascii_height * char_height;
    
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
    fprintf(fp, "viewBox=\"0 0 %d %d\" ", svg_width, svg_height);
    fprintf(fp, "width=\"%d\" height=\"%d\">\n", svg_width, svg_height);
    fprintf(fp, "<rect width=\"100%%\" height=\"100%%\" fill=\"#000000\"/>\n");
    fprintf(fp, "<style>text{font-family:'Courier New',Consolas,monospace;font-size:%dpx;dominant-baseline:text-before-edge;}</style>\n", char_height);
    
    char escaped[16];
    for (int y = 0; y < ctx->ascii_height; y++) {
        for (int x = 0; x < ctx->ascii_width; x++) {
            int idx = y * ctx->ascii_width + x;
            char c = ctx->char_buffer[idx];
            if (c == ' ') continue;
            
            unsigned char r = ctx->color_buffer[idx * 3 + 0];
            unsigned char g = ctx->color_buffer[idx * 3 + 1];
            unsigned char b = ctx->color_buffer[idx * 3 + 2];
            if (r < 5 && g < 5 && b < 5) continue;
            
            svg_escape_char(c, escaped);
            fprintf(fp, "<text x=\"%d\" y=\"%d\" fill=\"#%02x%02x%02x\">%s</text>\n",
                    x * char_width, y * char_height, r, g, b, escaped);
        }
    }
    
    fprintf(fp, "</svg>\n");
    fclose(fp);
    return 0;
}

// =====================================================================================
// V18: ANSI Export - Terminal-compatible colored text output
// Uses 24-bit ANSI escape codes for true color terminals
// =====================================================================================

int engine_render_to_ansi_file(ProcessingContext* ctx, const EngineConfig* config) {
    FILE* fp = fopen(config->output_filename, "w");
    if (!fp) { return -1; }
    
    // Write header comment
    fprintf(fp, "# Pixel-Ripper V18 ANSI Art\n");
    fprintf(fp, "# Dimensions: %dx%d\n", ctx->ascii_width, ctx->ascii_height);
    fprintf(fp, "# Use 'cat filename.ansi' to view in terminal\n\n");
    
    for (int y = 0; y < ctx->ascii_height; y++) {
        for (int x = 0; x < ctx->ascii_width; x++) {
            int idx = y * ctx->ascii_width + x;
            char c = ctx->char_buffer[idx];
            
            if (config->use_color) {
                unsigned char r = ctx->color_buffer[idx * 3 + 0];
                unsigned char g = ctx->color_buffer[idx * 3 + 1];
                unsigned char b = ctx->color_buffer[idx * 3 + 2];
                // 24-bit ANSI color: \x1b[38;2;R;G;Bm
                fprintf(fp, "\x1b[38;2;%d;%d;%dm%c", r, g, b, c);
            } else {
                fputc(c, fp);
            }
        }
        fprintf(fp, "\x1b[0m\n");  // Reset color at end of line
    }
    
    fprintf(fp, "\x1b[0m");  // Final reset
    fclose(fp);
    return 0;
}

// =====================================================================================
// V18: HTML Export - Web-embeddable ASCII art with CSS
// Responsive, dark-themed, uses span elements for colors
// =====================================================================================

int engine_render_to_html_file(ProcessingContext* ctx, const EngineConfig* config) {
    FILE* fp = fopen(config->output_filename, "w");
    if (!fp) { return -1; }
    
    // HTML header with responsive CSS
    fprintf(fp, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(fp, "<meta charset=\"UTF-8\">\n");
    fprintf(fp, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n");
    fprintf(fp, "<title>Pixel-Ripper ASCII Art</title>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "body { background: #0a0a0a; margin: 0; padding: 20px; display: flex; justify-content: center; }\n");
    fprintf(fp, ".ascii-art { font-family: 'Courier New', Consolas, Monaco, monospace; font-size: 8px; line-height: 1; white-space: pre; letter-spacing: 0; }\n");
    fprintf(fp, "@media (max-width: 800px) { .ascii-art { font-size: 4px; } }\n");
    fprintf(fp, "</style>\n");
    fprintf(fp, "</head>\n<body>\n");
    fprintf(fp, "<div class=\"ascii-art\">");
    
    for (int y = 0; y < ctx->ascii_height; y++) {
        for (int x = 0; x < ctx->ascii_width; x++) {
            int idx = y * ctx->ascii_width + x;
            char c = ctx->char_buffer[idx];
            
            // Escape HTML special chars
            const char* escaped;
            char single[2] = {c, '\0'};
            switch(c) {
                case '<': escaped = "&lt;"; break;
                case '>': escaped = "&gt;"; break;
                case '&': escaped = "&amp;"; break;
                case '"': escaped = "&quot;"; break;
                case ' ': escaped = "&nbsp;"; break;
                default: escaped = single; break;
            }
            
            if (config->use_color) {
                unsigned char r = ctx->color_buffer[idx * 3 + 0];
                unsigned char g = ctx->color_buffer[idx * 3 + 1];
                unsigned char b = ctx->color_buffer[idx * 3 + 2];
                fprintf(fp, "<span style=\"color:#%02x%02x%02x\">%s</span>", r, g, b, escaped);
            } else {
                fprintf(fp, "%s", escaped);
            }
        }
        fprintf(fp, "\n");
    }
    
    fprintf(fp, "</div>\n</body>\n</html>\n");
    fclose(fp);
    return 0;
}

static const char* init_encoder(ProcessingContext* ctx, const EngineConfig* config) {
    int out_width = ctx->ascii_width * 8;
    int out_height = ctx->ascii_height * 8;

    // Why this warning? H.264 levels have hard limits. 8K video is bleeding-edge.
    // We warn the user that their resulting file might not be playable on all
    // hardware or software, which is a critical piece of information.
    if (out_width > 7680 || out_height > 4320) {
        fprintf(stderr, "WARNING: Requested resolution (%dx%d) is extremely high and may exceed standard H.264 limits, potentially creating an incompatible file.\n", out_width, out_height);
    }

    avformat_alloc_output_context2(&ctx->enc_fmt_ctx, NULL, NULL, config->output_filename);
    if (!ctx->enc_fmt_ctx) { return "Could not create output context"; }

    ctx->enc_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!ctx->enc_codec) { return "H.264 encoder not found"; }
    
    AVStream* in_stream = ctx->dec_fmt_ctx->streams[ctx->video_stream_idx];
    ctx->out_video_stream = avformat_new_stream(ctx->enc_fmt_ctx, ctx->enc_codec);
    if (!ctx->out_video_stream) { return "Failed to create new video stream"; }
    ctx->enc_codec_ctx = avcodec_alloc_context3(ctx->enc_codec);
    if (!ctx->enc_codec_ctx) { return "Failed to create encoder context"; }

    ctx->enc_codec_ctx->height = out_height;
    ctx->enc_codec_ctx->width = out_width;
    ctx->enc_codec_ctx->sample_aspect_ratio = in_stream->codecpar->sample_aspect_ratio;
    ctx->enc_codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    ctx->enc_codec_ctx->time_base = in_stream->time_base;
    ctx->enc_codec_ctx->framerate = in_stream->r_frame_rate;

    if (ctx->enc_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        ctx->enc_codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // Why these options? This is where we seize control of the encoder.
    // 'preset': 'medium' is a much better balance of speed vs. compression than 'ultrafast'.
    // 'crf': This is the master quality control. We expose it directly to the user.
    // 'level': '6.2' is the highest H.264 level, necessary for >4K resolutions. We
    //          are telling the encoder we know what we're doing and to prepare for a massive frame.
    av_opt_set(ctx->enc_codec_ctx->priv_data, "preset", "medium", 0);
    av_opt_set_int(ctx->enc_codec_ctx->priv_data, "crf", config->crf, 0);
    av_opt_set(ctx->enc_codec_ctx->priv_data, "level", "6.2", 0);
    
    if (avcodec_open2(ctx->enc_codec_ctx, ctx->enc_codec, NULL) < 0) { return "Cannot open video encoder"; }
    if (avcodec_parameters_from_context(ctx->out_video_stream->codecpar, ctx->enc_codec_ctx) < 0) {
        return "Failed to copy encoder parameters to video stream";
    }
    ctx->out_video_stream->time_base = ctx->enc_codec_ctx->time_base;

    if (ctx->audio_stream_idx >= 0) {
        AVStream* in_audio_stream = ctx->dec_fmt_ctx->streams[ctx->audio_stream_idx];
        ctx->out_audio_stream = avformat_new_stream(ctx->enc_fmt_ctx, NULL);
        if (!ctx->out_audio_stream) { return "Failed to create new audio stream"; }
        if (avcodec_parameters_copy(ctx->out_audio_stream->codecpar, in_audio_stream->codecpar) < 0) {
            return "Failed to copy audio parameters";
        }
        ctx->out_audio_stream->time_base = in_audio_stream->time_base;
    }

    if (!(ctx->enc_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&ctx->enc_fmt_ctx->pb, config->output_filename, AVIO_FLAG_WRITE) < 0) { return "Could not open output file"; }
    }

    if (avformat_write_header(ctx->enc_fmt_ctx, NULL) < 0) { return "Error occurred when opening output file"; }

    ctx->yuv_frame = av_frame_alloc();
    ctx->yuv_frame->format = AV_PIX_FMT_YUV420P;
    ctx->yuv_frame->width = out_width;
    ctx->yuv_frame->height = out_height;
    if (av_image_alloc(ctx->yuv_frame->data, ctx->yuv_frame->linesize, out_width, out_height, AV_PIX_FMT_YUV420P, 1) < 0) {
        return "Failed to allocate YUV frame buffer. The requested resolution is likely too high for available memory.";
    }

    ctx->sws_ctx_to_yuv = sws_getContext(out_width, out_height, AV_PIX_FMT_RGB24,
                                        out_width, out_height, AV_PIX_FMT_YUV420P,
                                        SWS_BILINEAR, NULL, NULL, NULL);

    return NULL;
}

int engine_encode_video_frame(ProcessingContext* ctx, const struct AVFrame* original_frame, const EngineConfig* config) {
    int out_width = ctx->ascii_width * 8;
    int out_height = ctx->ascii_height * 8;
    unsigned char* rgb_buffer = (unsigned char*)arena_alloc(&ctx->frame_arena, (size_t)out_width * out_height * 3);
    if (!rgb_buffer) return -1;
    render_ascii_to_buffer(ctx, rgb_buffer, config);

    const uint8_t* const in_data[1] = { rgb_buffer };
    const int in_linesize[1] = { out_width * 3 };
    sws_scale(ctx->sws_ctx_to_yuv, in_data, in_linesize, 0, out_height, ctx->yuv_frame->data, ctx->yuv_frame->linesize);

    ctx->yuv_frame->pts = original_frame->pts;

    int ret = avcodec_send_frame(ctx->enc_codec_ctx, ctx->yuv_frame);
    if (ret < 0) { 
        fprintf(stderr, "Error sending frame to encoder.\n");
        return -1; 
    }

    AVPacket* pkt = av_packet_alloc();
    while (ret >= 0) {
        ret = avcodec_receive_packet(ctx->enc_codec_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
        if (ret < 0) { av_packet_free(&pkt); return -1; }

        pkt->stream_index = ctx->out_video_stream->index;
        av_interleaved_write_frame(ctx->enc_fmt_ctx, pkt);
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);
    return 0;
}

int engine_remux_packet(ProcessingContext* ctx, AVPacket* packet) {
    // Why remux? The audio stream doesn't need to be re-encoded. Doing so would
    // be a waste of CPU cycles and could degrade quality. Remuxing simply copies
    // the compressed audio packets from the input container to the output
    // container, modifying only the timestamps to ensure they stay in sync with
    // our newly generated video stream. It's the most efficient path.
    if (ctx->audio_stream_idx >= 0 && packet->stream_index == ctx->audio_stream_idx) {
        packet->stream_index = ctx->out_audio_stream->index;
        av_packet_rescale_ts(packet,
                             ctx->dec_fmt_ctx->streams[ctx->audio_stream_idx]->time_base,
                             ctx->out_audio_stream->time_base);
        return av_interleaved_write_frame(ctx->enc_fmt_ctx, packet);
    }
    return 0;
}

void engine_finalize_video_encoder(ProcessingContext* ctx) {
    // Why flush the encoder? Encoders often buffer several frames internally to
    // make better compression decisions (e.g., using B-frames). Sending a NULL
    // frame signals the end of the stream, forcing the encoder to output any
    // remaining buffered frames. Without this step, the last few frames of the
    // video would be lost.
    avcodec_send_frame(ctx->enc_codec_ctx, NULL);
    AVPacket* pkt = av_packet_alloc();
    int ret;
    while(1) {
        ret = avcodec_receive_packet(ctx->enc_codec_ctx, pkt);
        if (ret == AVERROR_EOF || ret < 0) break;
        av_interleaved_write_frame(ctx->enc_fmt_ctx, pkt);
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);
    av_write_trailer(ctx->enc_fmt_ctx);
}

int engine_get_video_stream_idx(const ProcessingContext* ctx) {
    return ctx->video_stream_idx;
}

int engine_get_audio_stream_idx(const ProcessingContext* ctx) {
    return ctx->audio_stream_idx;
}

float engine_get_video_aspect(const ProcessingContext* ctx) {
    if (!ctx || !ctx->dec_codec_ctx || ctx->dec_codec_ctx->height == 0) {
        return 16.0f / 9.0f;
    }
    return (float)ctx->dec_codec_ctx->width / (float)ctx->dec_codec_ctx->height;
}

void engine_update_output_dims(ProcessingContext* ctx, int new_ascii_width, int new_ascii_height) {
    if (!ctx) return;
    ctx->ascii_width = new_ascii_width;
    ctx->ascii_height = new_ascii_height;
}

