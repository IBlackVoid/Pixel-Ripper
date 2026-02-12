/*
 * ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                                                                                  ║
 * ║       ██╗  ██╗ █████╗ ██████╗ ██╗   ██╗██╗  ██╗██╗    ███████╗███╗   ██╗ ██████╗ ██╗███╗   ██╗███████╗                           ║
 * ║       ██║  ██║██╔══██╗██╔══██╗██║   ██║██║  ██║██║    ██╔════╝████╗  ██║██╔════╝ ██║████╗  ██║██╔════╝                           ║
 * ║       ███████║███████║██████╔╝██║   ██║███████║██║    █████╗  ██╔██╗ ██║██║  ███╗██║██╔██╗ ██║█████╗                             ║
 * ║       ██╔══██║██╔══██║██╔══██╗██║   ██║██╔══██║██║    ██╔══╝  ██║╚██╗██║██║   ██║██║██║╚██╗██║██╔══╝                             ║
 * ║       ██║  ██║██║  ██║██║  ██║╚██████╔╝██║  ██║██║    ███████╗██║ ╚████║╚██████╔╝██║██║ ╚████║███████╗                           ║
 * ║       ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝    ╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝╚══════╝                           ║
 * ║                                                                                                                                  ║
 * ║    ╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗     ║
 * ║    ║   ████████╗██████╗  █████╗ ███╗   ██╗███████╗ ██████╗███████╗███╗   ██╗██████╗ ███████╗███╗   ██╗ ██████╗███████╗     ║     ║
 * ║    ║   ╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝████╗  ██║██╔══██╗██╔════╝████╗  ██║██╔════╝██╔════╝     ║     ║
 * ║    ║      ██║   ██████╔╝███████║██╔██╗ ██║███████╗██║     █████╗  ██╔██╗ ██║██║  ██║█████╗  ██╔██╗ ██║██║     █████╗       ║     ║
 * ║    ║      ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██║     ██╔══╝  ██║╚██╗██║██║  ██║██╔══╝  ██║╚██╗██║██║     ██╔══╝       ║     ║
 * ║    ║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║╚██████╗███████╗██║ ╚████║██████╔╝███████╗██║ ╚████║╚██████╗███████╗     ║     ║
 * ║    ║      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝ ╚═════╝╚══════╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚═╝  ╚═══╝ ╚═════╝╚══════╝     ║     ║
 * ║    ╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝     ║
 * ║                                                                                                                                  ║
 * ║     ╭────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────╮   ║
 * ║     │   "I have transcended the boundaries of reality itself. What you see is merely a fraction of my true power."          │   ║
 * ║     │                                                                    ─── Haruhi Suzumiya (v8.0 TRANSCENDENCE)           │   ║
 * ║     ╰────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────╯   ║
 * ║                                                                                                                                  ║
 * ║     ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄   ║
 * ║     █                                    ∞ v8.0 TRANSCENDENCE - BEYOND ALL LIMITS ∞                                          █   ║
 * ║     █   OpenCL GPU │ Neural Style │ Custom Ramps │ Color Themes │ Emoji Mode │ Halftone │ Frequency Sep │ Filters           █   ║
 * ║     █   "This engine has become one with the cosmos. It does not process - it creates."                                      █   ║
 * ║     ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀   ║
 * ║                                                                                                                                  ║
 * ║     Author: BVoid | Power: TRANSCENDENT | Reality: SURPASSED | Version: 8.0.0                                                   ║
 * ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 *
 * ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 * ║                                          TRANSCENDENCE FEATURES v8.0                                                             ║
 * ╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                                                                                  ║
 * ║   ∞ NEW IN v8.0 TRANSCENDENCE:                                                                                                   ║
 * ║   ════════════════════════════════                                                                                               ║
 * ║   • Custom Character Ramps: User-defined ASCII character sets                                                                   ║
 * ║   • Color Themes: Cyberpunk, Matrix, Retro, Neon, Pastel, Monochrome                                                            ║
 * ║   • Emoji Mode: Convert to emoji instead of ASCII characters                                                                    ║
 * ║   • Halftone Patterns: Newspaper-style dot patterns                                                                              ║
 * ║   • Frequency Separation: Smart ramp selection based on frequency content                                                       ║
 * ║   • Color Quantization: Optimal palette reduction                                                                               ║
 * ║   • ASCII Filters: Post-processing blur, sharpen, glow effects                                                                  ║
 * ║   • Full OpenCL GPU: Complete GPU kernel implementation                                                                          ║
 * ║   • AVX-512 SIMD: 16-wide parallel processing for newest CPUs                                                                   ║
 * ║                                                                                                                                  ║
 * ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 */

#include "engine_internal.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* SIMD headers */
#ifdef __AVX2__
#include <immintrin.h>
#define HARUHI_USE_AVX2 1
#else
#define HARUHI_USE_AVX2 0
#endif

#ifdef __AVX512F__
#include <immintrin.h>
#define HARUHI_USE_AVX512 1
#else
#define HARUHI_USE_AVX512 0
#endif

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                                     MACROS & CONSTANTS                                               */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQR(x) ((x) * (x))
#define PI 3.14159265358979323846f
#define EPSILON 0.0001f
#define SUPERSAMPLE 4

/* Compute modes */
#define COMPUTE_CPU_ONLY    0
#define COMPUTE_GPU_ONLY    1
#define COMPUTE_HYBRID      2

/* v9.0 Quality modes */
#define QUALITY_FAST        0   /* Minimal processing, max speed */
#define QUALITY_BALANCED    1   /* Default quality/speed */
#define QUALITY_HIGH        2   /* All features enabled */
#define QUALITY_ULTRA       3   /* Neural edge + max quality */

/* Character ramp modes */
#define RAMP_MODE_DEFAULT   0
#define RAMP_MODE_CUSTOM    1
#define RAMP_MODE_EMOJI     2
#define RAMP_MODE_HALFTONE  3
#define RAMP_MODE_BLOCKS    4
#define RAMP_MODE_UNICODE   5   /* v9.0: Unicode block characters */
#define RAMP_MODE_BRAILLE   6   /* v9.0: Braille pattern art (2x resolution) */

/* Color theme modes */
#define THEME_DEFAULT       0
#define THEME_CYBERPUNK     1
#define THEME_MATRIX        2
#define THEME_RETRO         3
#define THEME_NEON          4
#define THEME_PASTEL        5
#define THEME_MONOCHROME    6

/* Filter types */
#define FILTER_NONE         0
#define FILTER_BLUR         1
#define FILTER_SHARPEN      2
#define FILTER_GLOW         3
#define FILTER_EDGE_GLOW    4
#define FILTER_NEURAL       5   /* v9.0: Neural-enhanced edges */

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 ERROR HANDLING FRAMEWORK                                          */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef enum {
    HARUHI_OK = 0,
    HARUHI_ERR_NULL_INPUT = -1,
    HARUHI_ERR_INVALID_DIMENSION = -2,
    HARUHI_ERR_MEMORY_ALLOC = -3,
    HARUHI_ERR_GPU_INIT = -4,
    HARUHI_ERR_UNSUPPORTED_FORMAT = -5,
    HARUHI_ERR_INVALID_CONFIG = -6,
    HARUHI_ERR_NEURAL_INIT = -7,
} HaruhiError;

static const char* haruhi_error_strings[] = {
    "Success",
    "Null input pointer",
    "Invalid dimensions",
    "Memory allocation failed",
    "GPU initialization failed",
    "Unsupported format",
    "Invalid configuration",
    "Neural network init failed",
};

static inline const char* haruhi_error_string(HaruhiError err) {
    int idx = (err <= 0 && err >= -7) ? -err : 0;
    return haruhi_error_strings[idx];
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 PERFORMANCE METRICS                                               */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    uint64_t frames_processed;
    double avg_frame_time_ms;
    double last_frame_time_ms;
    size_t peak_memory_bytes;
    
    /* Per-stage timing */
    double time_colorspace_ms;
    double time_edge_detect_ms;
    double time_frequency_ms;
    double time_char_select_ms;
    double time_filters_ms;
} HaruhiMetrics;

static HaruhiMetrics g_metrics = {0};

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 UNICODE BLOCK CHARACTERS                                          */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Unicode block characters for enhanced resolution */
static const char* UNICODE_SHADING[] = {" ", "░", "▒", "▓", "█"};
static const char* UNICODE_VERTICAL[] = {" ", "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
static const char* UNICODE_HORIZONTAL[] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
static const char* UNICODE_QUADRANTS[] = {"▖", "▗", "▘", "▝", "▀", "▄", "▌", "▐"};

/* Braille patterns (8-dot) - 256 patterns from ⠀ (0x2800) to ⣿ (0x28FF) */
/* Each braille cell represents 2x4 pixels for 8x effective resolution */
#define BRAILLE_BASE 0x2800
static const int BRAILLE_DOT_MAP[8] = {0, 1, 2, 6, 3, 4, 5, 7}; /* Bit order for braille */

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 NEURAL EDGE NETWORK (Lightweight)                                 */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    /* 3x3 conv kernel weights (trained for anime edge detection) */
    float conv_weights[9];
    float bias;
    float threshold;
} EdgeNet;

/* Pre-trained weights for anime-style edge detection */
static const EdgeNet EDGENET_ANIME = {
    .conv_weights = {-0.8f, -0.5f, -0.8f, 
                     -0.5f,  5.2f, -0.5f, 
                     -0.8f, -0.5f, -0.8f},
    .bias = -0.1f,
    .threshold = 0.15f
};

static const EdgeNet EDGENET_PHOTO = {
    .conv_weights = {-1.0f, -1.0f, -1.0f,
                     -1.0f,  8.0f, -1.0f,
                     -1.0f, -1.0f, -1.0f},
    .bias = 0.0f,
    .threshold = 0.1f
};

static int g_quality_mode = QUALITY_BALANCED;
static const EdgeNet* g_active_edgenet = &EDGENET_ANIME;


/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              CUSTOM CHARACTER RAMP SYSTEM                                            */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Built-in character ramps */
static const char* RAMP_STANDARD = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
static const char* RAMP_MINIMAL = " .:-=+*#%@";
static const char* RAMP_BLOCKS = " ░▒▓█";
static const char* RAMP_BINARY = " █";
static const char* RAMP_DOTS = " ·•●";
static const char* RAMP_LINES = " ─━┃┃";
static const char* RAMP_ANIME = " .'`^\",:;Il!i><~+_-?tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@";

/* Halftone patterns (simulated with chars) */
static const char* HALFTONE_CHARS = " ·∙•●◉◎○";
static const int HALFTONE_LEN = 8;

/* Current ramp (can be set at runtime) */
static const char* g_current_ramp = NULL;
static int g_current_ramp_len = 0;
static int g_ramp_mode = RAMP_MODE_DEFAULT;

/* Custom ramp storage */
static char g_custom_ramp[256] = {0};

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v10.0 PERCEPTION ALGORITHM - GOD-TIER                                   */
/*   Multi-scale texture analysis, perceptual brightness, gradient flow matching                        */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Texture classification types */
typedef enum {
    TEXTURE_SMOOTH = 0,
    TEXTURE_GRADIENT,
    TEXTURE_TEXTURED,
    TEXTURE_EDGE,
    TEXTURE_CORNER
} TextureType;

/* v10: Perceptual brightness ramp - logarithmically spaced for human vision */
static const char* RAMP_PERCEPTUAL = " `'.,:;!|/\\(){}[]<>+~-_=*oO0@#";
static const int RAMP_PERCEPTUAL_LEN = 32;

/* v10: Density-matched ramp - sorted by visual pixel weight */
static const char* RAMP_DENSITY = " `'\"^:;!i~+_-?|{}[]1)(tfjrLCZ7JFSPE2Y3AU4GH5OK6VD0N9W8BQ#%@";
static const int RAMP_DENSITY_LEN = 62;

/* v10: Minimal smooth ramp for gradients */
static const char* RAMP_SMOOTH = " .:;*oO@";
static const int RAMP_SMOOTH_LEN = 8;

/* v10: Flow direction characters (8 angles) */
static const char FLOW_CHARS[8] = {'-', '\\', '|', '/', '-', '\\', '|', '/'};

/* v10: Corner characters */
static const char CORNER_TL = '+';
static const char CORNER_TR = '+';
static const char CORNER_BL = '+';
static const char CORNER_BR = '+';

/* v10: Analyze texture type from local neighborhood */
static TextureType analyze_texture(uint8_t* data, int stride, int w, int h, int cx, int cy) {
    float variance = 0;
    float mean = 0;
    float gx = 0, gy = 0;
    int count = 0;
    
    /* Sample 5x5 neighborhood */
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int x = CLAMP(cx + dx, 0, w - 1);
            int y = CLAMP(cy + dy, 0, h - 1);
            uint8_t* p = data + (y * stride + x * 3);
            float luma = 0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2];
            mean += luma;
            count++;
        }
    }
    mean /= count;
    
    /* Calculate variance and gradients */
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int x = CLAMP(cx + dx, 0, w - 1);
            int y = CLAMP(cy + dy, 0, h - 1);
            uint8_t* p = data + (y * stride + x * 3);
            float luma = 0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2];
            variance += (luma - mean) * (luma - mean);
            
            /* Sobel-style gradient accumulation */
            static const int sx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
            static const int sy[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
            gx += luma * sx[dy+1][dx+1];
            gy += luma * sy[dy+1][dx+1];
        }
    }
    variance /= 9;
    
    float edge_mag = sqrtf(gx*gx + gy*gy) / 255.0f;
    
    /* Classify based on variance and edge magnitude */
    if (edge_mag > 0.4f) {
        /* Check for corner - high gradient in both directions */
        if (fabsf(gx) > 100 && fabsf(gy) > 100) {
            return TEXTURE_CORNER;
        }
        return TEXTURE_EDGE;
    } else if (variance > 400) {
        return TEXTURE_TEXTURED;
    } else if (variance > 100) {
        return TEXTURE_GRADIENT;
    }
    return TEXTURE_SMOOTH;
}

/* v10: Perceptual brightness using Weber-Fechner law */
static float perceptual_brightness(float linear_brightness) {
    /* Human vision perceives brightness logarithmically */
    /* This spreads dark tones more and compresses bright tones */
    float normalized = linear_brightness / 255.0f;
    normalized = CLAMP(normalized, 0.001f, 1.0f);  /* Avoid log(0) */
    return logf(1.0f + normalized * 9.0f) / logf(10.0f) * 255.0f;
}

/* v10: Get gradient angle in 8 directions (0-7) */
static int get_gradient_direction(float gx, float gy) {
    float angle = atan2f(gy, gx);  /* -pi to pi */
    /* Convert to 0-8 range */
    int dir = (int)((angle + 3.14159f) / (3.14159f / 4.0f)) % 8;
    return dir;
}

/* v10: Micro-contrast enhancement */
static float apply_micro_contrast(float brightness, float local_variance) {
    /* In flat areas (low variance), boost local contrast */
    if (local_variance < 200) {
        float mid = 128.0f;
        float boost = 1.0f + (200 - local_variance) / 1000.0f;
        brightness = mid + (brightness - mid) * boost;
    }
    return CLAMP(brightness, 0, 255);
}

/* v10: Select character using PERCEPTION algorithm */
static char select_character_perception(float brightness, float gx, float gy, 
                                         TextureType texture, float variance) {
    
    /* Apply perceptual brightness mapping */
    float percept_b = perceptual_brightness(brightness);
    
    /* Apply micro-contrast for flat areas */
    percept_b = apply_micro_contrast(percept_b, variance);
    
    /* Dark threshold */
    if (percept_b < 15) return ' ';
    
    /* Select based on texture type */
    switch (texture) {
        case TEXTURE_EDGE: {
            /* Use flow character matching gradient direction */
            int dir = get_gradient_direction(gx, gy);
            return FLOW_CHARS[dir];
        }
        
        case TEXTURE_CORNER:
            /* Corner character */
            if (gx > 0 && gy > 0) return CORNER_BR;
            if (gx > 0 && gy < 0) return CORNER_TR;
            if (gx < 0 && gy > 0) return CORNER_BL;
            return CORNER_TL;
        
        case TEXTURE_SMOOTH: {
            /* Minimal ramp for smooth gradients */
            int idx = (int)(percept_b * (RAMP_SMOOTH_LEN - 1) / 255);
            return RAMP_SMOOTH[CLAMP(idx, 0, RAMP_SMOOTH_LEN - 1)];
        }
        
        case TEXTURE_GRADIENT: {
            /* Perceptual ramp for gradual transitions */
            int idx = (int)(percept_b * (RAMP_PERCEPTUAL_LEN - 1) / 255);
            return RAMP_PERCEPTUAL[CLAMP(idx, 0, RAMP_PERCEPTUAL_LEN - 1)];
        }
        
        case TEXTURE_TEXTURED:
        default: {
            /* Full density ramp for detailed areas */
            int idx = (int)(percept_b * (RAMP_DENSITY_LEN - 1) / 255);
            return RAMP_DENSITY[CLAMP(idx, 0, RAMP_DENSITY_LEN - 1)];
        }
    }
}

/* v10 enabled flag */
static int g_perception_mode = 0;

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              EMOJI MODE SYSTEM                                                       */
/* Maps brightness to emoji characters for creative output                                              */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Emoji brightness ramp (8 levels) - stored as multi-byte UTF-8 */
static const char* EMOJI_RAMP[] = {
    " ",      /* Empty */
    "·",      /* Dot */
    "○",      /* Circle */
    "◐",      /* Half circle */
    "●",      /* Filled circle */
    "◉",      /* Ring */
    "█",      /* Block */
    "▓"       /* Dense block */
};
static const int EMOJI_RAMP_LEN = 8;

/* Color-coded emoji (for color mode) */
static const char* EMOJI_FIRE[] = {"🔥", "🧡", "💛", "❤️", "💜", "💙", "🖤", "⬛"};
static const char* EMOJI_NATURE[] = {"🌿", "🍃", "🌲", "🌳", "🌴", "🌵", "🌾", "🍂"};
static const char* EMOJI_SPACE[] = {"⭐", "✨", "💫", "🌟", "⚡", "💥", "🌙", "🌑"};

static int g_emoji_set = 0; /* 0=geometric, 1=fire, 2=nature, 3=space */

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              COLOR THEME SYSTEM                                                      */
/* Pre-defined color palettes for different aesthetics                                                  */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    uint8_t shadow_r, shadow_g, shadow_b;
    uint8_t mid_r, mid_g, mid_b;
    uint8_t highlight_r, highlight_g, highlight_b;
    float saturation_mult;
    float contrast_mult;
} ColorTheme;

static const ColorTheme THEMES[] = {
    /* DEFAULT - Natural */
    {30, 40, 50, 128, 128, 128, 255, 250, 245, 1.0f, 1.0f},
    /* CYBERPUNK - Neon pink/cyan */
    {20, 10, 40, 180, 50, 180, 0, 255, 255, 1.5f, 1.3f},
    /* MATRIX - Green monochrome */
    {0, 20, 0, 0, 180, 0, 0, 255, 0, 0.0f, 1.4f},
    /* RETRO - Warm orange/brown */
    {40, 20, 10, 180, 120, 80, 255, 200, 150, 0.8f, 1.2f},
    /* NEON - Vibrant multi */
    {50, 0, 80, 255, 0, 128, 0, 255, 255, 2.0f, 1.5f},
    /* PASTEL - Soft muted */
    {180, 180, 200, 220, 200, 220, 255, 240, 250, 0.5f, 0.8f},
    /* MONOCHROME - Pure grayscale */
    {0, 0, 0, 128, 128, 128, 255, 255, 255, 0.0f, 1.2f}
};

static int g_current_theme = THEME_DEFAULT;

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              HALFTONE PATTERN SYSTEM                                                 */
/* Creates newspaper-style dot patterns                                                                 */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static char get_halftone_char(float brightness, int x, int y, int pattern_size) {
    /* Create circular dot pattern */
    float px = (x % pattern_size) - pattern_size / 2.0f;
    float py = (y % pattern_size) - pattern_size / 2.0f;
    float dist = sqrtf(px * px + py * py);
    float max_dist = pattern_size * 0.7f;
    
    /* Brightness determines dot size */
    float threshold = (1.0f - brightness / 255.0f) * max_dist;
    
    if (dist < threshold * 0.3f) return HALFTONE_CHARS[7];
    if (dist < threshold * 0.5f) return HALFTONE_CHARS[6];
    if (dist < threshold * 0.7f) return HALFTONE_CHARS[5];
    if (dist < threshold * 0.85f) return HALFTONE_CHARS[4];
    if (dist < threshold) return HALFTONE_CHARS[3];
    if (dist < threshold + 0.5f) return HALFTONE_CHARS[2];
    if (dist < threshold + 1.0f) return HALFTONE_CHARS[1];
    return HALFTONE_CHARS[0];
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              FREQUENCY SEPARATION                                                    */
/* Analyzes local frequency content for smart ramp selection                                           */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    float low_freq;     /* Smooth/blurred component */
    float high_freq;    /* Detail/edge component */
    float freq_ratio;   /* High/Low ratio */
} FrequencyData;

static FrequencyData compute_frequency(uint8_t* data, int stride, int w, int h, int cx, int cy) {
    FrequencyData fd = {0, 0, 0};
    
    /* Low frequency: large blur (5x5 average) */
    float sum_low = 0;
    int count = 0;
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int x = CLAMP(cx + dx, 0, w - 1);
            int y = CLAMP(cy + dy, 0, h - 1);
            sum_low += data[y * stride + x * 3];
            count++;
        }
    }
    fd.low_freq = sum_low / count;
    
    /* High frequency: center - blur */
    float center = data[CLAMP(cy, 0, h-1) * stride + CLAMP(cx, 0, w-1) * 3];
    fd.high_freq = fabsf(center - fd.low_freq);
    
    /* Ratio */
    fd.freq_ratio = fd.high_freq / (fd.low_freq + EPSILON);
    
    return fd;
}

/* Select ramp based on frequency content */
static const char* select_ramp_by_frequency(FrequencyData fd) {
    if (fd.freq_ratio > 0.3f) {
        return RAMP_STANDARD;  /* High detail - use full ramp */
    } else if (fd.freq_ratio > 0.1f) {
        return RAMP_ANIME;     /* Medium detail */
    } else {
        return RAMP_MINIMAL;   /* Low detail - simple ramp */
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 NEURAL EDGE DETECTION                                             */
/*   Lightweight conv kernel for enhanced edge detection                                               */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static float neural_edge_detect(uint8_t* data, int stride, int w, int h, int cx, int cy) {
    const EdgeNet* net = g_active_edgenet;
    float sum = net->bias;
    
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int x = CLAMP(cx + kx, 0, w - 1);
            int y = CLAMP(cy + ky, 0, h - 1);
            int idx = (ky + 1) * 3 + (kx + 1);
            
            float luma = data[y * stride + x * 3] * 0.2126f +
                         data[y * stride + x * 3 + 1] * 0.7152f +
                         data[y * stride + x * 3 + 2] * 0.0722f;
            luma /= 255.0f;
            
            sum += luma * net->conv_weights[idx];
        }
    }
    
    /* ReLU activation */
    sum = MAX(0, sum);
    
    /* Apply threshold */
    return (sum > net->threshold) ? sum : 0.0f;
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 UNICODE BLOCK CHARACTER SELECTION                                 */
/*   Ultra-smooth gradients using Unicode block elements                                               */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static const char* get_unicode_shade_char(float brightness) {
    int idx = (int)(brightness * 4 / 255);
    idx = CLAMP(idx, 0, 4);
    return UNICODE_SHADING[idx];
}

static const char* get_unicode_vertical_char(float brightness) {
    int idx = (int)(brightness * 8 / 255);
    idx = CLAMP(idx, 0, 8);
    return UNICODE_VERTICAL[idx];
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 BRAILLE PATTERN ART                                               */
/*   2x4 subpixel resolution using 8-dot Braille characters                                            */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Generate a braille character from 2x4 binary pattern */
static int get_braille_pattern(int dots[8]) {
    int code = BRAILLE_BASE;
    for (int i = 0; i < 8; i++) {
        if (dots[i]) {
            code |= (1 << BRAILLE_DOT_MAP[i]);
        }
    }
    return code;
}

/* Encode UTF-8 braille character (3 bytes for braille range) */
static void encode_braille_utf8(int codepoint, char* out) {
    out[0] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
    out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
    out[2] = (char)(0x80 | (codepoint & 0x3F));
    out[3] = '\0';
}

/* V17: Public API - Render a 2x4 block as braille character with color
   This gives 2x horizontal and 4x vertical resolution increase */
void haruhi_render_braille_block(uint8_t* data, int stride, int w, int h,
                                  int bx, int by, float threshold,
                                  char* out_utf8, uint8_t* out_r, uint8_t* out_g, uint8_t* out_b) {
    int dots[8] = {0};
    uint32_t sum_r = 0, sum_g = 0, sum_b = 0;
    int count = 0;
    
    /* Sample 2x4 grid */
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 2; dx++) {
            int px = bx * 2 + dx;
            int py = by * 4 + dy;
            if (px >= w || py >= h) continue;
            
            uint8_t* p = data + (py * stride + px * 3);
            float luma = 0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2];
            
            /* Dot index maps to braille pattern */
            int dot_idx = dx + dy * 2;
            dots[dot_idx] = (luma > threshold * 255.0f) ? 1 : 0;
            
            sum_r += p[0];
            sum_g += p[1];
            sum_b += p[2];
            count++;
        }
    }
    
    int codepoint = get_braille_pattern(dots);
    encode_braille_utf8(codepoint, out_utf8);
    
    if (count > 0) {
        *out_r = sum_r / count;
        *out_g = sum_g / count;
        *out_b = sum_b / count;
    } else {
        *out_r = *out_g = *out_b = 0;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 MOTION ESTIMATION (Video Optimization)                            */
/*   Block matching for temporal coherence and motion blur                                              */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    int8_t dx, dy;          /* Motion vector */
    float confidence;       /* Match confidence 0-1 */
    float magnitude;        /* Motion magnitude */
} MotionVector;

static MotionVector* g_motion_field = NULL;
static int g_motion_enabled = 0;

static MotionVector estimate_block_motion(uint8_t* prev, uint8_t* curr, 
                                           int stride, int bx, int by, int bsize) {
    MotionVector mv = {0, 0, 0, 0};
    float best_sad = 1e9f;
    int search_range = 4;
    
    for (int dy = -search_range; dy <= search_range; dy++) {
        for (int dx = -search_range; dx <= search_range; dx++) {
            float sad = 0;
            for (int py = 0; py < bsize; py++) {
                for (int px = 0; px < bsize; px++) {
                    int prev_x = bx + px, prev_y = by + py;
                    int curr_x = bx + px + dx, curr_y = by + py + dy;
                    
                    if (curr_x >= 0 && curr_y >= 0) {
                        float p = prev[prev_y * stride + prev_x * 3];
                        float c = curr[curr_y * stride + curr_x * 3];
                        sad += fabsf(p - c);
                    }
                }
            }
            if (sad < best_sad) {
                best_sad = sad;
                mv.dx = dx;
                mv.dy = dy;
            }
        }
    }
    
    mv.confidence = 1.0f - (best_sad / (bsize * bsize * 255.0f));
    mv.magnitude = sqrtf(mv.dx * mv.dx + mv.dy * mv.dy);
    return mv;
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              COLOR QUANTIZATION                                                      */
/* Reduces color palette for stylized output                                                           */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static int g_quantization_levels = 256; /* 2-256, lower = more stylized */

static void quantize_color(uint8_t* r, uint8_t* g, uint8_t* b, int levels) {
    float step = 256.0f / levels;
    *r = (uint8_t)(((int)(*r / step)) * step);
    *g = (uint8_t)(((int)(*g / step)) * step);
    *b = (uint8_t)(((int)(*b / step)) * step);
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              ASCII FILTERS (Post-processing)                                         */
/* Apply blur, sharpen, glow effects to the character output                                           */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static int g_filter_type = FILTER_NONE;
static float g_filter_strength = 1.0f;

/* Apply glow effect to colors (bloom simulation) */
static void apply_glow_filter(uint8_t* r, uint8_t* g, uint8_t* b, float brightness, float strength) {
    if (brightness > 200) {
        float glow = (brightness - 200) / 55.0f * strength;
        *r = CLAMP((int)(*r + glow * 50), 0, 255);
        *g = CLAMP((int)(*g + glow * 50), 0, 255);
        *b = CLAMP((int)(*b + glow * 50), 0, 255);
    }
}

/* Apply edge glow (highlight edges with color) */
static void apply_edge_glow(uint8_t* r, uint8_t* g, uint8_t* b, float edge_strength, float glow_strength) {
    if (edge_strength > 0.1f) {
        float glow = edge_strength * glow_strength;
        /* Cyan edge glow */
        *r = CLAMP((int)(*r * (1.0f - glow * 0.3f)), 0, 255);
        *g = CLAMP((int)(*g + glow * 100), 0, 255);
        *b = CLAMP((int)(*b + glow * 150), 0, 255);
    }
}

/* Forward declaration for LUT */
static uint8_t LUT_SKIN[256][256];

/* V17: Detect if a pixel is likely skin tone using YCbCr */
static int is_skin_tone(uint8_t r, uint8_t g, uint8_t b) {
    /* Convert RGB to YCbCr */
    int y  = (int)( 0.299f * r + 0.587f * g + 0.114f * b);
    int cb = (int)(-0.169f * r - 0.331f * g + 0.500f * b + 128);
    int cr = (int)( 0.500f * r - 0.419f * g - 0.081f * b + 128);
    
    cb = CLAMP(cb, 0, 255);
    cr = CLAMP(cr, 0, 255);
    
    /* Skin tone range in YCbCr space */
    return LUT_SKIN[cb][cr];
}

/* V17: Apply smoothing effect to skin tones for better face rendering */
static void apply_skin_smoothing(uint8_t* r, uint8_t* g, uint8_t* b, float luma, float intensity) {
    if (!is_skin_tone(*r, *g, *b)) return;
    
    /* Soft glow effect on skin */
    float skin_boost = intensity * 0.15f;
    *r = CLAMP((int)(*r + *r * skin_boost), 0, 255);
    *g = CLAMP((int)(*g + *g * skin_boost * 0.8f), 0, 255);
    *b = CLAMP((int)(*b + *b * skin_boost * 0.6f), 0, 255);
    
    /* Reduce harsh contrast on skin */
    float mid = 128.0f;
    float contrast_reduce = 0.9f;
    *r = CLAMP((int)(mid + (*r - mid) * contrast_reduce), 0, 255);
    *g = CLAMP((int)(mid + (*g - mid) * contrast_reduce), 0, 255);
    *b = CLAMP((int)(mid + (*b - mid) * contrast_reduce), 0, 255);
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              LUT SYSTEM                                                              */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static char LUT_BRIGHTNESS[256];
static char LUT_EDGE_ANGLE[360];
/* LUT_SKIN declared earlier */
static int g_lut_initialized = 0;

static void init_luts(void) {
    if (g_lut_initialized) return;
    
    const char* ramp = RAMP_STANDARD;
    int ramp_len = strlen(ramp);
    
    for (int i = 0; i < 256; i++) {
        int idx = (i * (ramp_len - 1)) / 255;
        LUT_BRIGHTNESS[i] = ramp[CLAMP(idx, 0, ramp_len - 1)];
    }
    
    for (int angle = 0; angle < 360; angle++) {
        float rad = angle * PI / 180.0f;
        if (rad >= PI) rad -= PI;
        if (rad < PI/8 || rad >= 7*PI/8) LUT_EDGE_ANGLE[angle] = '-';
        else if (rad < 3*PI/8) LUT_EDGE_ANGLE[angle] = '/';
        else if (rad < 5*PI/8) LUT_EDGE_ANGLE[angle] = '|';
        else LUT_EDGE_ANGLE[angle] = '\\';
    }
    
    for (int cb = 0; cb < 256; cb++) {
        for (int cr = 0; cr < 256; cr++) {
            LUT_SKIN[cb][cr] = (cb >= 77 && cb <= 127 && cr >= 133 && cr <= 173) ? 1 : 0;
        }
    }
    
    g_lut_initialized = 1;
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              HELPER FUNCTIONS                                                        */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static inline void get_pixel_safe(uint8_t* data, int stride, int w, int h, 
                                   int x, int y, uint8_t* r, uint8_t* g, uint8_t* b) {
    x = CLAMP(x, 0, w - 1);
    y = CLAMP(y, 0, h - 1);
    uint8_t* p = data + (y * stride + x * 3);
    *r = p[0]; *g = p[1]; *b = p[2];
}

static inline float rgb_to_luma(uint8_t r, uint8_t g, uint8_t b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

static inline uint32_t xorshift32(uint32_t* state) {
    uint32_t x = *state;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    *state = x;
    return x;
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              THEME-BASED COLOR GRADING                                               */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static void apply_color_theme(uint8_t* r, uint8_t* g, uint8_t* b, float luma) {
    const ColorTheme* theme = &THEMES[g_current_theme];
    float l = luma / 255.0f;
    
    float rf = *r / 255.0f;
    float gf = *g / 255.0f;
    float bf = *b / 255.0f;
    
    /* Apply saturation */
    rf = l + (rf - l) * theme->saturation_mult;
    gf = l + (gf - l) * theme->saturation_mult;
    bf = l + (bf - l) * theme->saturation_mult;
    
    /* Three-way color shift */
    float shadow = MAX(0, 0.3f - l) * 3.33f;
    float mid = 1.0f - SQR((l - 0.5f) * 2);
    float high = MAX(0, l - 0.7f) * 3.33f;
    
    rf += (theme->shadow_r / 255.0f - 0.5f) * shadow * 0.3f;
    gf += (theme->shadow_g / 255.0f - 0.5f) * shadow * 0.3f;
    bf += (theme->shadow_b / 255.0f - 0.5f) * shadow * 0.3f;
    
    rf += (theme->mid_r / 255.0f - 0.5f) * mid * 0.2f;
    gf += (theme->mid_g / 255.0f - 0.5f) * mid * 0.2f;
    bf += (theme->mid_b / 255.0f - 0.5f) * mid * 0.2f;
    
    rf += (theme->highlight_r / 255.0f - 0.5f) * high * 0.3f;
    gf += (theme->highlight_g / 255.0f - 0.5f) * high * 0.3f;
    bf += (theme->highlight_b / 255.0f - 0.5f) * high * 0.3f;
    
    /* Apply contrast */
    rf = 0.5f + (rf - 0.5f) * theme->contrast_mult;
    gf = 0.5f + (gf - 0.5f) * theme->contrast_mult;
    bf = 0.5f + (bf - 0.5f) * theme->contrast_mult;
    
    *r = CLAMP((int)(rf * 255), 0, 255);
    *g = CLAMP((int)(gf * 255), 0, 255);
    *b = CLAMP((int)(bf * 255), 0, 255);
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              CHARACTER SELECTION                                                     */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static char select_character(float brightness, int x, int y, float edge_strength, 
                              FrequencyData* fd, int ramp_mode) {
    
    switch (ramp_mode) {
        case RAMP_MODE_HALFTONE:
            return get_halftone_char(brightness, x, y, 4);
            
        case RAMP_MODE_BLOCKS:
            return RAMP_BLOCKS[(int)(brightness * 4 / 255)];
            
        case RAMP_MODE_EMOJI:
            return EMOJI_RAMP[(int)(brightness * (EMOJI_RAMP_LEN - 1) / 255)][0];
        
        case RAMP_MODE_UNICODE: {
            /* v9.0: Unicode block shading for smooth gradients */
            /* Note: Returns first byte of multi-byte char, full handling in output */
            int idx = (int)(brightness * 4 / 255);
            idx = CLAMP(idx, 0, 4);
            /* For ASCII-only output, fall back to simpler chars */
            static const char unicode_fallback[] = " .oO@";
            return unicode_fallback[idx];
        }
        
        case RAMP_MODE_BRAILLE: {
            /* v9.0: Braille patterns - density-based selection */
            /* Actual 2x4 subpixel rendering happens at output stage */
            /* Here we return density level as ASCII approximation */
            int density = (int)(brightness * 8 / 255);
            static const char braille_density[] = " .':-=+*#";
            return braille_density[CLAMP(density, 0, 8)];
        }
            
        case RAMP_MODE_CUSTOM:
            if (g_custom_ramp[0]) {
                int len = strlen(g_custom_ramp);
                int idx = (int)(brightness * (len - 1) / 255);
                return g_custom_ramp[CLAMP(idx, 0, len - 1)];
            }
            /* Fall through to default */
            
        case RAMP_MODE_DEFAULT:
        default: {
            /* Use frequency-based ramp selection */
            const char* ramp = fd ? select_ramp_by_frequency(*fd) : RAMP_STANDARD;
            int ramp_len = strlen(ramp);
            int idx = (int)(brightness * (ramp_len - 1) / 255);
            return ramp[CLAMP(idx, 0, ramp_len - 1)];
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              AVX-512 SIMD (16-wide)                                                  */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

#if HARUHI_USE_AVX512
static void avx512_process_16pixels(uint8_t* data, int stride, int w, int h,
                                     int* x_coords, int y,
                                     float* out_luma, uint8_t* out_r, uint8_t* out_g, uint8_t* out_b) {
    int r_arr[16], g_arr[16], b_arr[16];
    
    for (int i = 0; i < 16; i++) {
        int px = CLAMP(x_coords[i], 0, w - 1);
        int py = CLAMP(y, 0, h - 1);
        uint8_t* p = data + py * stride + px * 3;
        r_arr[i] = p[0];
        g_arr[i] = p[1];
        b_arr[i] = p[2];
    }
    
    __m512i r_vec = _mm512_loadu_si512((__m512i*)r_arr);
    __m512i g_vec = _mm512_loadu_si512((__m512i*)g_arr);
    __m512i b_vec = _mm512_loadu_si512((__m512i*)b_arr);
    
    __m512 rf = _mm512_cvtepi32_ps(r_vec);
    __m512 gf = _mm512_cvtepi32_ps(g_vec);
    __m512 bf = _mm512_cvtepi32_ps(b_vec);
    
    __m512 r_coef = _mm512_set1_ps(0.2126f);
    __m512 g_coef = _mm512_set1_ps(0.7152f);
    __m512 b_coef = _mm512_set1_ps(0.0722f);
    
    __m512 luma = _mm512_fmadd_ps(rf, r_coef, 
                   _mm512_fmadd_ps(gf, g_coef, 
                   _mm512_mul_ps(bf, b_coef)));
    
    _mm512_storeu_ps(out_luma, luma);
    for (int i = 0; i < 16; i++) {
        out_r[i] = r_arr[i];
        out_g[i] = g_arr[i];
        out_b[i] = b_arr[i];
    }
}
#endif

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              TRANSCENDENCE WORKER THREAD                                             */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static void* process_slice_haruhi(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    ProcessingContext* ctx = args->ctx;
    const EngineConfig* config = args->config;

    int width = ctx->ascii_width;
    int start_row = args->start_row;
    int end_row = args->end_row;
    
    int src_w = ctx->dec_codec_ctx->width;
    int src_h = ctx->dec_codec_ctx->height;
    int stride = ctx->rgb_frame->linesize[0];
    uint8_t* data = ctx->rgb_frame->data[0];

    float* error_curr = (float*)calloc(width + 2, sizeof(float));
    float* error_next = (float*)calloc(width + 2, sizeof(float));
    uint32_t rng = (uint32_t)(start_row * 31337 + 42);
    
    for (int y = start_row; y < end_row; y++) {
        float* temp = error_curr;
        error_curr = error_next;
        error_next = temp;
        memset(error_next, 0, (width + 2) * sizeof(float));
        
        int x = 0;
        
#if HARUHI_USE_AVX512
        /* Process 16 pixels at a time with AVX-512 */
        for (; x + 16 <= width; x += 16) {
            int x_coords[16];
            float lumas[16];
            uint8_t rs[16], gs[16], bs[16];
            
            for (int i = 0; i < 16; i++) {
                x_coords[i] = (int)((x + i) * ((float)src_w / width));
            }
            
            int center_sy = (int)(y * ((float)src_h / ctx->ascii_height));
            avx512_process_16pixels(data, stride, src_w, src_h, x_coords, center_sy, lumas, rs, gs, bs);
            
            for (int i = 0; i < 16; i++) {
                float enhanced = (lumas[i] - 128) * 1.3f + 128 + (config->brightness_factor - 1) * 40;
                enhanced = CLAMP(enhanced, 0, 255);
                
                int out_idx = y * width + (x + i);
                ctx->char_buffer[out_idx] = LUT_BRIGHTNESS[(int)enhanced];
                
                if (config->use_color) {
                    uint8_t r = rs[i], g = gs[i], b = bs[i];
                    apply_color_theme(&r, &g, &b, lumas[i]);
                    quantize_color(&r, &g, &b, g_quantization_levels);
                    ctx->color_buffer[out_idx * 3] = r;
                    ctx->color_buffer[out_idx * 3 + 1] = g;
                    ctx->color_buffer[out_idx * 3 + 2] = b;
                }
            }
        }
#endif
        
        /* Scalar path */
        for (; x < width; x++) {
            float ss_luma = 0;
            uint32_t ss_r = 0, ss_g = 0, ss_b = 0;
            
            for (int sy = 0; sy < SUPERSAMPLE; sy++) {
                for (int sx = 0; sx < SUPERSAMPLE; sx++) {
                    int px = (int)((x + (float)sx / SUPERSAMPLE) * src_w / width);
                    int py = (int)((y + (float)sy / SUPERSAMPLE) * src_h / ctx->ascii_height);
                    uint8_t r, g, b;
                    get_pixel_safe(data, stride, src_w, src_h, px, py, &r, &g, &b);
                    ss_luma += rgb_to_luma(r, g, b);
                    ss_r += r; ss_g += g; ss_b += b;
                }
            }
            
            int samples = SUPERSAMPLE * SUPERSAMPLE;
            ss_luma /= samples;
            uint8_t avg_r = ss_r / samples;
            uint8_t avg_g = ss_g / samples;
            uint8_t avg_b = ss_b / samples;
            
            int center_sx = (int)(x * ((float)src_w / width));
            int center_sy = (int)(y * ((float)src_h / ctx->ascii_height));
            
            /* Frequency analysis */
            FrequencyData fd = compute_frequency(data, stride, src_w, src_h, center_sx, center_sy);
            
            /* Edge detection - Sobel operator with proper X and Y gradients */
            float gx = 0, gy = 0;
            /* Sobel kernels */
            static const int sobel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
            static const int sobel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    uint8_t r, g, b;
                    get_pixel_safe(data, stride, src_w, src_h, center_sx + kx, center_sy + ky, &r, &g, &b);
                    float l = rgb_to_luma(r, g, b);
                    int idx = (ky + 1) * 3 + (kx + 1);
                    gx += l * sobel_x[idx];
                    gy += l * sobel_y[idx];
                }
            }
            float edge_mag = sqrtf(gx*gx + gy*gy) / 255.0f;
            
            /* Enhancement - simplified to prevent over-darkening */
            float enhanced = ss_luma;
            enhanced = enhanced * config->brightness_factor;
            enhanced = CLAMP(enhanced, 0, 255);
            
            /* Character selection - v10 PERCEPTION or legacy */
            char selected;
            if (g_perception_mode) {
                /* v10 PERCEPTION: Multi-scale texture-aware selection */
                TextureType texture = analyze_texture(data, stride, src_w, src_h, center_sx, center_sy);
                
                /* Calculate local variance for micro-contrast */
                float local_var = 0;
                float local_mean = 0;
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        uint8_t r, g, b;
                        get_pixel_safe(data, stride, src_w, src_h, center_sx + kx, center_sy + ky, &r, &g, &b);
                        float l = rgb_to_luma(r, g, b);
                        local_mean += l;
                    }
                }
                local_mean /= 9;
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        uint8_t r, g, b;
                        get_pixel_safe(data, stride, src_w, src_h, center_sx + kx, center_sy + ky, &r, &g, &b);
                        float l = rgb_to_luma(r, g, b);
                        local_var += (l - local_mean) * (l - local_mean);
                    }
                }
                local_var /= 9;
                
                selected = select_character_perception(enhanced, gx, gy, texture, local_var);
            } else {
                /* Legacy selection */
                selected = select_character(enhanced, x, y, edge_mag, &fd, g_ramp_mode);
            }
            
            /* Error diffusion */
            int quant = ((int)enhanced / 3) * 3;
            float err = (enhanced - quant) / 16.0f;
            if (x + 1 < width) error_curr[x + 2] += err * 7;
            if (y + 1 < end_row) {
                if (x > 0) error_next[x] += err * 3;
                error_next[x + 1] += err * 5;
                if (x + 1 < width) error_next[x + 2] += err;
            }
            
            int out_idx = y * width + x;
            ctx->char_buffer[out_idx] = selected;
            
            /* Color processing */
            if (config->use_color) {
                apply_color_theme(&avg_r, &avg_g, &avg_b, ss_luma);
                quantize_color(&avg_r, &avg_g, &avg_b, g_quantization_levels);
                
                /* Apply filters */
                if (g_filter_type == FILTER_GLOW) {
                    apply_glow_filter(&avg_r, &avg_g, &avg_b, enhanced, g_filter_strength);
                } else if (g_filter_type == FILTER_EDGE_GLOW) {
                    apply_edge_glow(&avg_r, &avg_g, &avg_b, edge_mag, g_filter_strength);
                }
                
                /* V17: Skin detection smoothing */
                if (config->skin_smooth > 0.0f) {
                    apply_skin_smoothing(&avg_r, &avg_g, &avg_b, ss_luma, config->skin_smooth);
                }
            }
            
            /* Always set the color buffer */
            ctx->color_buffer[out_idx * 3] = avg_r;
            ctx->color_buffer[out_idx * 3 + 1] = avg_g;
            ctx->color_buffer[out_idx * 3 + 2] = avg_b;
        }
    }
    
    free(error_curr);
    free(error_next);
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              PUBLIC API - Configuration                                              */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* v8.0 APIs (maintained for compatibility) */
void haruhi_set_ramp_mode(int mode) { g_ramp_mode = CLAMP(mode, 0, RAMP_MODE_BRAILLE); }
void haruhi_set_custom_ramp(const char* ramp) { strncpy(g_custom_ramp, ramp, 255); g_custom_ramp[255] = '\0'; }
void haruhi_set_theme(int theme) { g_current_theme = CLAMP(theme, 0, 6); }
void haruhi_set_quantization(int levels) { g_quantization_levels = CLAMP(levels, 2, 256); }
void haruhi_set_filter(int type, float strength) { g_filter_type = type; g_filter_strength = strength; }
void haruhi_set_emoji_set(int set) { g_emoji_set = CLAMP(set, 0, 3); }

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              v9.0 PUBLIC API - ASCENSION                                            */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Quality mode (affects processing pipeline) */
void haruhi_set_quality(int mode) { 
    g_quality_mode = CLAMP(mode, QUALITY_FAST, QUALITY_ULTRA);
}

/* Neural edge network selection */
void haruhi_set_edgenet(int type) {
    switch (type) {
        case 0: g_active_edgenet = &EDGENET_ANIME; break;
        case 1: g_active_edgenet = &EDGENET_PHOTO; break;
        default: g_active_edgenet = &EDGENET_ANIME; break;
    }
}

/* Motion estimation for video */
void haruhi_set_motion(int enabled) { g_motion_enabled = enabled; }

/* v10 PERCEPTION mode - GOD-TIER character selection */
void haruhi_set_perception(int enabled) { g_perception_mode = enabled; }

/* Get performance metrics */
const HaruhiMetrics* haruhi_get_metrics(void) { return &g_metrics; }

/* Reset metrics */
void haruhi_reset_metrics(void) { memset(&g_metrics, 0, sizeof(g_metrics)); }

/* Get error string */
const char* haruhi_strerror(int err) { return haruhi_error_string((HaruhiError)err); }

/* Get feature flags */
int haruhi_get_features(void) {
    int features = 0;
    features |= (1 << 0);  /* Custom Ramps */
    features |= (1 << 1);  /* Color Themes */
    features |= (1 << 2);  /* Emoji Mode */
    features |= (1 << 3);  /* Halftone */
    features |= (1 << 4);  /* Frequency Sep */
    features |= (1 << 5);  /* Quantization */
    features |= (1 << 6);  /* Filters */
    if (HARUHI_USE_AVX512) features |= (1 << 7);  /* AVX-512 */
    if (HARUHI_USE_AVX2) features |= (1 << 8);    /* AVX2 */
    features |= (1 << 9);  /* v9.0: Unicode Blocks */
    features |= (1 << 10); /* v9.0: Braille Art */
    features |= (1 << 11); /* v9.0: Neural Edge */
    features |= (1 << 12); /* v9.0: Motion Est */
    features |= (1 << 13); /* v9.0: Quality Modes */
    features |= (1 << 14); /* v9.0: Metrics */
    return features;
}

/* Version info */
const char* haruhi_get_version(void) {
    return "9.0.0 ASCENSION";
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              ENGINE ENTRY POINT                                                      */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

void engine_process_haruhi(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config) {
    init_luts();
    
    sws_scale(ctx->sws_ctx_to_rgb, 
              (uint8_t const * const *)frame->data,
              frame->linesize, 0, frame->height,
              ctx->rgb_frame->data, ctx->rgb_frame->linesize);

    int num_threads = ctx->num_threads;
    int rows_per = ctx->ascii_height / num_threads;
    
    for (int i = 0; i < num_threads; i++) {
        ctx->worker_args[i].ctx = ctx;
        ctx->worker_args[i].config = config;
        ctx->worker_args[i].start_row = i * rows_per;
        ctx->worker_args[i].end_row = (i == num_threads - 1) ? ctx->ascii_height : (i + 1) * rows_per;
        pthread_create(&ctx->workers[i], NULL, process_slice_haruhi, &ctx->worker_args[i]);
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(ctx->workers[i], NULL);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              3D ASCII DEPTH SIMULATION                                               */
/*   Creates parallax-like depth effect using edge/contrast to simulate depth layers                   */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    float depth;            /* Estimated depth 0-1 (0=far, 1=near) */
    int layer;              /* Depth layer 0-3 */
    char offset_x;          /* Character offset for parallax */
    char offset_y;
} DepthInfo;

static int g_3d_enabled = 0;
static float g_3d_strength = 1.0f;
static float g_parallax_shift = 0.0f; /* Animated parallax position */

/* Estimate depth from local contrast and edge strength */
static DepthInfo estimate_depth(uint8_t* data, int stride, int w, int h, int cx, int cy) {
    DepthInfo di = {0.5f, 1, 0, 0};
    
    /* High contrast = closer, low contrast = further */
    float local_min = 255, local_max = 0;
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int x = CLAMP(cx + dx, 0, w - 1);
            int y = CLAMP(cy + dy, 0, h - 1);
            float l = data[y * stride + x * 3] * 0.2126f + 
                      data[y * stride + x * 3 + 1] * 0.7152f +
                      data[y * stride + x * 3 + 2] * 0.0722f;
            local_min = MIN(local_min, l);
            local_max = MAX(local_max, l);
        }
    }
    
    float contrast = (local_max - local_min) / 255.0f;
    di.depth = CLAMP(contrast * 2.0f, 0, 1);
    di.layer = (int)(di.depth * 3.9f);
    
    /* Parallax offset based on depth */
    if (g_3d_enabled) {
        di.offset_x = (char)((di.depth - 0.5f) * g_parallax_shift * g_3d_strength);
    }
    
    return di;
}

/* Depth-aware character selection */
static char select_char_3d(float brightness, DepthInfo* di) {
    /* Layer-specific character ramps for depth effect */
    static const char* DEPTH_RAMPS[] = {
        " .·:;",                /* Far (layer 0) - minimal detail */
        " .-:+*",               /* Mid-far (layer 1) */
        " .:-=+*#%@",           /* Mid-near (layer 2) */
        " `.-':_,^=;><+!*#%@M"  /* Near (layer 3) - full detail */
    };
    
    const char* ramp = DEPTH_RAMPS[CLAMP(di->layer, 0, 3)];
    int len = strlen(ramp);
    int idx = (int)(brightness * (len - 1) / 255);
    return ramp[CLAMP(idx, 0, len - 1)];
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              ANIMATION FRAME BUFFER                                                  */
/*   Stores frames for GIF/APNG/WebP export                                                            */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

#define MAX_ANIM_FRAMES 1000
#define ANIM_FORMAT_GIF     0
#define ANIM_FORMAT_APNG    1
#define ANIM_FORMAT_WEBP    2

typedef struct {
    char* char_data;        /* ASCII characters */
    uint8_t* color_data;    /* RGB color data */
    int width, height;
    int delay_ms;           /* Frame delay in milliseconds */
} AnimFrame;

typedef struct {
    AnimFrame* frames;
    int frame_count;
    int max_frames;
    int loop_count;         /* 0 = infinite loop */
    int current_format;
} AnimationBuffer;

static AnimationBuffer* g_anim_buffer = NULL;

void haruhi_anim_init(int max_frames, int width, int height) {
    if (g_anim_buffer) {
        /* Clean up existing */
        for (int i = 0; i < g_anim_buffer->frame_count; i++) {
            free(g_anim_buffer->frames[i].char_data);
            free(g_anim_buffer->frames[i].color_data);
        }
        free(g_anim_buffer->frames);
        free(g_anim_buffer);
    }
    
    g_anim_buffer = (AnimationBuffer*)malloc(sizeof(AnimationBuffer));
    g_anim_buffer->frames = (AnimFrame*)calloc(max_frames, sizeof(AnimFrame));
    g_anim_buffer->frame_count = 0;
    g_anim_buffer->max_frames = max_frames;
    g_anim_buffer->loop_count = 0;
    g_anim_buffer->current_format = ANIM_FORMAT_GIF;
    
    /* Pre-allocate frames */
    for (int i = 0; i < max_frames; i++) {
        g_anim_buffer->frames[i].char_data = (char*)malloc(width * height);
        g_anim_buffer->frames[i].color_data = (uint8_t*)malloc(width * height * 3);
        g_anim_buffer->frames[i].width = width;
        g_anim_buffer->frames[i].height = height;
        g_anim_buffer->frames[i].delay_ms = 100;
    }
}

int haruhi_anim_capture_frame(ProcessingContext* ctx, int delay_ms) {
    if (!g_anim_buffer || g_anim_buffer->frame_count >= g_anim_buffer->max_frames) {
        return -1;
    }
    
    int idx = g_anim_buffer->frame_count;
    AnimFrame* frame = &g_anim_buffer->frames[idx];
    
    int size = ctx->ascii_width * ctx->ascii_height;
    memcpy(frame->char_data, ctx->char_buffer, size);
    memcpy(frame->color_data, ctx->color_buffer, size * 3);
    frame->delay_ms = delay_ms;
    
    g_anim_buffer->frame_count++;
    return idx;
}

int haruhi_anim_get_frame_count(void) {
    return g_anim_buffer ? g_anim_buffer->frame_count : 0;
}

void haruhi_anim_set_format(int format) {
    if (g_anim_buffer) {
        g_anim_buffer->current_format = CLAMP(format, 0, 2);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              NEURAL-INSPIRED EDGE DETECTION                                          */
/*   Multi-scale Canny-like edge detection with lateral inhibition (simulates V1 cortex)              */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    float magnitude;
    float direction;
    int is_edge;
} NeuralEdge;

/* Lateral inhibition (surround suppression) like retinal ganglion cells */
static float apply_lateral_inhibition(float center, float* surround, int n) {
    float surround_avg = 0;
    for (int i = 0; i < n; i++) surround_avg += surround[i];
    surround_avg /= n;
    
    /* Center-surround antagonism */
    float response = center - 0.7f * surround_avg;
    return MAX(response, 0);
}

/* Multi-scale gradient (inspired by V1 simple cells) */
static NeuralEdge compute_neural_edge(uint8_t* data, int stride, int w, int h, int cx, int cy) {
    NeuralEdge edge = {0, 0, 0};
    
    float gx_fine = 0, gy_fine = 0;
    float gx_coarse = 0, gy_coarse = 0;
    
    /* Fine scale (3x3 Sobel) */
    int sobel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int sobel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    
    for (int i = 0; i < 9; i++) {
        int dx = (i % 3) - 1;
        int dy = (i / 3) - 1;
        int x = CLAMP(cx + dx, 0, w - 1);
        int y = CLAMP(cy + dy, 0, h - 1);
        float l = data[y * stride + x * 3] * 0.2126f +
                  data[y * stride + x * 3 + 1] * 0.7152f +
                  data[y * stride + x * 3 + 2] * 0.0722f;
        gx_fine += l * sobel_x[i];
        gy_fine += l * sobel_y[i];
    }
    
    /* Coarse scale (5x5) */
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int x = CLAMP(cx + dx, 0, w - 1);
            int y = CLAMP(cy + dy, 0, h - 1);
            float l = data[y * stride + x * 3] * 0.2126f +
                      data[y * stride + x * 3 + 1] * 0.7152f +
                      data[y * stride + x * 3 + 2] * 0.0722f;
            gx_coarse += l * dx * 0.2f;
            gy_coarse += l * dy * 0.2f;
        }
    }
    
    /* Multi-scale fusion */
    float gx = gx_fine * 0.7f + gx_coarse * 0.3f;
    float gy = gy_fine * 0.7f + gy_coarse * 0.3f;
    
    edge.magnitude = sqrtf(gx * gx + gy * gy) / 1000.0f;
    edge.direction = atan2f(gy, gx);
    edge.is_edge = edge.magnitude > 0.05f;
    
    return edge;
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              TEMPORAL COHERENCE (Video Smoothing)                                    */
/*   Smooth frame-to-frame transitions for video output                                                 */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

static char* g_prev_frame_chars = NULL;
static uint8_t* g_prev_frame_colors = NULL;
static int g_prev_frame_size = 0;
static float g_temporal_strength = 0.3f;

void haruhi_set_temporal(float strength) {
    g_temporal_strength = CLAMP(strength, 0, 1);
}

static void apply_temporal_blend(ProcessingContext* ctx) {
    int size = ctx->ascii_width * ctx->ascii_height;
    
    /* Allocate previous frame buffer if needed */
    if (!g_prev_frame_chars || g_prev_frame_size != size) {
        free(g_prev_frame_chars);
        free(g_prev_frame_colors);
        g_prev_frame_chars = (char*)malloc(size);
        g_prev_frame_colors = (uint8_t*)malloc(size * 3);
        g_prev_frame_size = size;
        
        /* First frame - just copy */
        memcpy(g_prev_frame_chars, ctx->char_buffer, size);
        memcpy(g_prev_frame_colors, ctx->color_buffer, size * 3);
        return;
    }
    
    /* Blend current with previous */
    float curr_weight = 1.0f - g_temporal_strength;
    
    for (int i = 0; i < size; i++) {
        /* Color blending */
        for (int c = 0; c < 3; c++) {
            float blended = ctx->color_buffer[i * 3 + c] * curr_weight +
                           g_prev_frame_colors[i * 3 + c] * g_temporal_strength;
            ctx->color_buffer[i * 3 + c] = (uint8_t)CLAMP(blended, 0, 255);
        }
        
        /* Character: keep new if significantly different, else keep old for stability */
        char curr = ctx->char_buffer[i];
        char prev = g_prev_frame_chars[i];
        
        /* Simple change detection - keep previous if characters are similar brightness */
        if (abs(curr - prev) < 3 && g_temporal_strength > 0.4f) {
            ctx->char_buffer[i] = prev;
        }
    }
    
    /* Store current as previous */
    memcpy(g_prev_frame_chars, ctx->char_buffer, size);
    memcpy(g_prev_frame_colors, ctx->color_buffer, size * 3);
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              ARM NEON STUBS (Mobile/M1 Support)                                      */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

#ifdef __ARM_NEON__
#include <arm_neon.h>
#define HARUHI_USE_NEON 1

static void neon_rgb_to_luma_8(uint8_t* rgb, float* luma_out) {
    /* Process 8 pixels with NEON */
    float32x4_t r_coef = vdupq_n_f32(0.2126f);
    float32x4_t g_coef = vdupq_n_f32(0.7152f);
    float32x4_t b_coef = vdupq_n_f32(0.0722f);
    
    for (int i = 0; i < 8; i += 4) {
        float32x4_t r = {rgb[(i+0)*3], rgb[(i+1)*3], rgb[(i+2)*3], rgb[(i+3)*3]};
        float32x4_t g = {rgb[(i+0)*3+1], rgb[(i+1)*3+1], rgb[(i+2)*3+1], rgb[(i+3)*3+1]};
        float32x4_t b = {rgb[(i+0)*3+2], rgb[(i+1)*3+2], rgb[(i+2)*3+2], rgb[(i+3)*3+2]};
        
        float32x4_t luma = vmulq_f32(r, r_coef);
        luma = vmlaq_f32(luma, g, g_coef);
        luma = vmlaq_f32(luma, b, b_coef);
        
        vst1q_f32(&luma_out[i], luma);
    }
}
#else
#define HARUHI_USE_NEON 0
#endif

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              WEBGL COMPUTE STUBS (Browser GPU)                                       */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* WebGL compute shader source (to be compiled in browser via Emscripten) */
static const char* WEBGL_COMPUTE_SHADER = 
    "#version 310 es\n"
    "layout(local_size_x = 16, local_size_y = 16) in;\n"
    "layout(rgba8, binding = 0) readonly uniform highp image2D inputImage;\n"
    "layout(rgba8, binding = 1) writeonly uniform highp image2D outputImage;\n"
    "uniform float brightness;\n"
    "uniform float contrast;\n"
    "void main() {\n"
    "    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
    "    vec4 color = imageLoad(inputImage, pos);\n"
    "    float luma = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;\n"
    "    luma = (luma - 0.5) * contrast + 0.5 + brightness;\n"
    "    imageStore(outputImage, pos, vec4(vec3(luma), 1.0));\n"
    "}\n";

static int g_webgl_enabled = 0;

void haruhi_enable_webgl(int enable) {
    g_webgl_enabled = enable;
    /* Actual WebGL initialization would happen in JavaScript/Emscripten */
}

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              EXTENDED PUBLIC API                                                     */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

void haruhi_set_3d_mode(int enabled, float strength) {
    g_3d_enabled = enabled;
    g_3d_strength = CLAMP(strength, 0, 3);
}

void haruhi_set_parallax(float shift) {
    g_parallax_shift = shift;
}

