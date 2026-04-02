// // /* arch/x86_64/avx2/resize_avx2.c */

// #include <immintrin.h>
// #include "arch/arch_interface.h"
// #include "plugins/plugin_resize.h"

// #include <immintrin.h>

// void img_arch_avx2_resize(img_ctx_t *ctx, img_buffer_t *dst, void *params)
// {
//     (void)ctx;

//     resize_params_t *p = (resize_params_t *)params;
//     img_buffer_t *src = p->src;

//     uint8_t *src_data = src->data;
//     uint8_t *dst_data = dst->data;

//     uint32_t scale_x = p->scale_x;
//     uint32_t scale_y = p->scale_y;

//     const uint32_t ch = src->channels;

//     for (uint32_t dy = 0; dy < dst->height; dy++)
//     {
//         uint32_t sy = (dy * scale_y) >> 16;

//         uint8_t *src_row = src_data + sy * src->stride;
//         uint8_t *dst_row = dst_data + dy * dst->stride;

//         uint32_t dx = 0;

//         // 🔥 SIMD LOOP (process 32 bytes)
//         for (; dx + 32 <= dst->width * ch; dx += 32)
//         {
//             uint32_t pixel_index = dx / ch;
//             uint32_t sx = (pixel_index * scale_x) >> 16;

//             __m256i pixels = _mm256_loadu_si256(
//                 (__m256i *)(src_row + sx * ch));

//             _mm256_storeu_si256(
//                 (__m256i *)(dst_row + dx),
//                 pixels);
//         }

//         // 🔥 TAIL (remaining pixels)
//         for (; dx < dst->width * ch; dx++)
//         {
//             uint32_t pixel_index = dx / ch;
//             uint32_t sx = (pixel_index * scale_x) >> 16;

//             dst_row[dx] = src_row[sx * ch + (dx % ch)];
//         }
//     }
// }

// // this is for avx2
// #include <immintrin.h>
// #include "arch/arch_interface.h"
// #include "plugins/plugin_resize.h"

// void img_arch_avx2_resize(img_ctx_t *ctx, img_buffer_t *dst, void *params)
// {
//     (void)ctx;

//     resize_params_t *p = (resize_params_t *)params;
//     img_buffer_t *src = p->src;

//     uint8_t *src_data = src->data;
//     uint8_t *dst_data = dst->data;

//     uint32_t scale_x = p->scale_x;
//     uint32_t scale_y = p->scale_y;

//     for (uint32_t dy = 0; dy < dst->height; dy++)
//     {
//         uint32_t sy = (dy * scale_y) >> 16;

//         uint8_t *src_row = src_data + sy * src->stride;
//         uint8_t *dst_row = dst_data + dy * dst->stride;

//         uint32_t dx = 0;

//         // 🔥 SIMD LOOP (8 pixels at once)
//         for (; dx + 8 <= dst->width; dx += 8)
//         {
//             __m256i dx_vec = _mm256_setr_epi32(
//                 dx, dx + 1, dx + 2, dx + 3,
//                 dx + 4, dx + 5, dx + 6, dx + 7);

//             __m256i scale = _mm256_set1_epi32(scale_x);

//             __m256i sx_vec = _mm256_mullo_epi32(dx_vec, scale);
//             sx_vec = _mm256_srli_epi32(sx_vec, 16);

//             // 🔥 Gather 8 pixels (grayscale-safe)
//             __m256i gathered = _mm256_i32gather_epi32(
//                 (int const *)src_row,
//                 sx_vec,
//                 1);

//             // Pack to bytes
//             __m256i packed = _mm256_packus_epi32(gathered, gathered);
//             packed = _mm256_packus_epi16(packed, packed);

//             // Store 8 pixels
//             *(uint64_t *)(dst_row + dx) =
//                 _mm_cvtsi128_si64(_mm256_castsi256_si128(packed));
//         }

//         // 🔥 TAIL LOOP
//         for (; dx < dst->width; dx++)
//         {
//             uint32_t sx = (dx * scale_x) >> 16;
//             dst_row[dx] = src_row[sx];
//         }
//     }
// }

#include <immintrin.h>
#include "plugins/plugin_resize.h"
#include "arch/arch_interface.h"

void img_arch_avx2_resize(img_ctx_t *ctx, img_buffer_t *dst, void *params)
{
    (void)ctx;

    resize_params_t *p = (resize_params_t *)params;
    img_buffer_t *src = p->src;

    uint8_t *src_data = src->data;
    uint8_t *dst_data = dst->data;

    uint32_t scale_x = p->scale_x;
    uint32_t scale_y = p->scale_y;

    const uint32_t ch = src->channels;

    for (uint32_t dy = 0; dy < dst->height; dy++)
    {
        uint32_t sy = (dy * scale_y) >> 16;

        uint8_t *src_row = src_data + sy * src->stride;
        uint8_t *dst_row = dst_data + dy * dst->stride;

        uint32_t sx_fixed = 0; // 🔥 incremental fixed-point
        uint32_t dx = 0;

        // 🔥 SIMD LOOP (32 bytes = best throughput)
        for (; dx + 32 <= dst->width * ch; dx += 32)
        {
            uint32_t sx = sx_fixed >> 16;

            // 🔥 CONTIGUOUS LOAD (FAST PATH)
            __m256i pixels = _mm256_loadu_si256(
                (__m256i *)(src_row + sx * ch));

            _mm256_storeu_si256(
                (__m256i *)(dst_row + dx),
                pixels);

            // 🔥 advance scale (no multiply!)
            sx_fixed += (scale_x * (32 / ch));
        }

        // 🔥 SCALAR FALLBACK
        for (; dx < dst->width * ch; dx++)
        {
            uint32_t pixel = dx / ch;
            uint32_t sx = (pixel * scale_x) >> 16;

            dst_row[dx] = src_row[sx * ch + (dx % ch)];
        }
    }
}