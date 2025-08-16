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
    // Why these ramps? The selection and order of characters are critical for perceived
    // brightness and texture. This ramp was carefully chosen to provide a smooth
    // gradient from dark/sparse to bright/dense characters.
    const char* flat_ramp = "`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
    const int flat_ramp_len = 90;
    // Why so few characters for edges? For edges, we want to represent direction, not
    // brightness. A minimal set of characters that form lines is sufficient and
    // prevents visual noise.
    const char* vertical_ramp = "|¦"; const int vertical_ramp_len = 2;
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

        // Pre-calculating the gamma curve. This is a one-time cost at initialization
        // that pays dividends on every single frame processed.
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

            char selected_char;
            if (mag_sq < edge_strength_sq) {
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

            // Why do saturation math in floating point? It provides more precision
            // and avoids clipping/overflow issues inherent in integer math. We convert
            // to HSV-like space (by finding luma), apply the saturation factor,
            // and then convert back, clamping only at the very end.
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

            unsigned char r = (r_f > 255.0f) ? 255 : (unsigned char)r_f;
            unsigned char g = (g_f > 255.0f) ? 255 : (unsigned char)g_f;
            unsigned char b = (b_f > 255.0f) ? 255 : (unsigned char)b_f;

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

    if (stbi_write_png(config->output_filename, out_img_width, out_img_height, 3, out_img_data, out_img_width * 3) == 0) {
        free(out_img_data);
        return -1;
    }
    
    free(out_img_data);
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

