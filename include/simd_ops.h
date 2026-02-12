/*
 * =====================================================================================
 *
 * Filename:  simd_ops.h
 *
 * Description:  SIMD (SSSE3) accelerated operations for the engine.
 * This is where we speak directly to the silicon.
 *
 * =====================================================================================
 */

#ifndef SIMD_OPS_H
#define SIMD_OPS_H

#include <emmintrin.h> // SSE2
#include <tmmintrin.h> // SSSE3 for _mm_shuffle_epi8

// Converts 8 interleaved RGB pixels (24 bytes) into 8 separate luma values (float).
// This is the heart of the SIMD pipeline. We load 16 bytes (5.33 pixels), process 4,
// then load the next 16, and so on. This requires careful handling of the data stream.
static inline __m128i load_and_interleave_8_pixels_to_3_planes(const uint8_t* p, __m128i* r, __m128i* g, __m128i* b) {
    // This is a complex shuffle operation to de-interleave RGBRGB... data.
    // It takes a pointer and loads 32 bytes to extract 8 full pixels into R, G, B planes.
    __m128i p0 = _mm_loadu_si128((__m128i*)(p));
    __m128i p1 = _mm_loadu_si128((__m128i*)(p + 16));
    
    // Shuffle masks to extract R, G, B channels from the loaded vectors.
    __m128i r_mask = _mm_setr_epi8(0, 3, 6, 9, 12, 15, 1, 4, 7, 10, 13, -1, -1, -1, -1, -1);
    __m128i g_mask = _mm_setr_epi8(1, 4, 7, 10, 13, 0, 2, 5, 8, 11, 14, -1, -1, -1, -1, -1);
    __m128i b_mask = _mm_setr_epi8(2, 5, 8, 11, 14, 1, 3, 6, 9, 12, 15, -1, -1, -1, -1, -1);

    __m128i r_shuf = _mm_shuffle_epi8(p0, r_mask);
    __m128i g_shuf = _mm_shuffle_epi8(p0, g_mask);
    __m128i b_shuf = _mm_shuffle_epi8(p0, b_mask);

    __m128i r_shuf_hi = _mm_shuffle_epi8(p1, _mm_srli_si128(r_mask, 5));
    __m128i g_shuf_hi = _mm_shuffle_epi8(p1, _mm_srli_si128(g_mask, 5));
    __m128i b_shuf_hi = _mm_shuffle_epi8(p1, _mm_srli_si128(b_mask, 5));

    *r = _mm_or_si128(r_shuf, _mm_slli_si128(r_shuf_hi, 6));
    *g = _mm_or_si128(g_shuf, _mm_slli_si128(g_shuf_hi, 6));
    *b = _mm_or_si128(b_shuf, _mm_slli_si128(b_shuf_hi, 5));

    return _mm_loadu_si128((__m128i*)(p + 24)); // Return the start of the next block
}


// Converts 8 unsigned 8-bit integers to 8 single-precision floats.
static inline void convert_uint8_to_float(__m128i in, __m128* out_lo, __m128* out_hi) {
    __m128i zero = _mm_setzero_si128();
    __m128i lo = _mm_unpacklo_epi8(in, zero);
    __m128i hi = _mm_unpackhi_epi8(in, zero);
    *out_lo = _mm_cvtepi32_ps(_mm_unpacklo_epi16(lo, zero));
    *out_hi = _mm_cvtepi32_ps(_mm_unpackhi_epi16(hi, zero));
}

#endif // SIMD_OPS_H

