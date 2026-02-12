#ifndef ENGINE_VARIANTS_H
#define ENGINE_VARIANTS_H

#include "ascii_engine.h"

// Common interface for all engine variants
// They all adhere to the "process frame" contract

void engine_process_speedster(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);
void engine_process_tiny(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);
void engine_process_artist(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);
void engine_process_parallel(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);
void engine_process_robust(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);
void engine_process_bvoid(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);
void engine_process_haruhi(ProcessingContext* ctx, const AVFrame* frame, const EngineConfig* config);

/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */
/*                              HARUHI v9.0 ASCENSION - Configuration API                              */
/* ═══════════════════════════════════════════════════════════════════════════════════════════════════ */

/* Ramp modes: 0=DEFAULT, 1=BLOCKS, 2=EMOJI, 3=HALFTONE, 4=CUSTOM, 5=UNICODE, 6=BRAILLE */
void haruhi_set_ramp_mode(int mode);
void haruhi_set_custom_ramp(const char* ramp);

/* Color themes: 0=DEFAULT, 1=CYBERPUNK, 2=MATRIX, 3=RETRO, 4=NEON, 5=PASTEL, 6=MONOCHROME */
void haruhi_set_theme(int theme);

/* Color quantization: 2-256 levels */
void haruhi_set_quantization(int levels);

/* Filters: 0=NONE, 1=GLOW, 2=EDGE_GLOW, 3=NEURAL */
void haruhi_set_filter(int type, float strength);

/* Quality modes: 0=FAST, 1=BALANCED, 2=HIGH, 3=ULTRA */
void haruhi_set_quality(int mode);

/* EdgeNet type: 0=ANIME, 1=PHOTO */
void haruhi_set_edgenet(int type);

/* Motion estimation for video */
void haruhi_set_motion(int enabled);

/* Get version and features */
const char* haruhi_get_version(void);
int haruhi_get_features(void);

#endif // ENGINE_VARIANTS_H
