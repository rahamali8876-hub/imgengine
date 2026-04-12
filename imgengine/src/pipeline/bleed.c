// src/pipeline/bleed.c

#define _GNU_SOURCE

#include "core/buffer.h"
#include "pipeline/layout.h"
#include "api/v1/img_error.h"
#include <string.h>

img_result_t img_apply_bleed(
    img_buffer_t *canvas,
    const img_layout_t *layout,
    uint32_t bleed_px)
{
    if (!canvas || !layout || bleed_px == 0)
        return IMG_SUCCESS;

    const uint32_t ch = 3;
    const int32_t cw = (int32_t)canvas->width;
    const int32_t cht = (int32_t)canvas->height;
    const uint32_t str = canvas->stride;

    /* canvas buffer end — never write past this */
    const uint8_t *buf_end = canvas->data + (size_t)cht * str;

    for (uint32_t i = 0; i < layout->count; i++)
    {
        const img_cell_t *c = &layout->cells[i];

        const int32_t cx = (int32_t)c->x;
        const int32_t cy = (int32_t)c->y;
        const int32_t cw2 = (int32_t)c->w;
        const int32_t ch2 = (int32_t)c->h;
        const int32_t bp = (int32_t)bleed_px;

        /* ── TOP bleed ── */
        for (int32_t b = 1; b <= bp; b++)
        {
            int32_t ty = cy - b;
            if (ty < 0)
                break;

            const uint8_t *src = canvas->data + (size_t)cy * str + (size_t)cx * ch;
            uint8_t *dst = canvas->data + (size_t)ty * str + (size_t)cx * ch;

            /* clamp copy width */
            int32_t cpw = cw2;
            if (cx + cpw > cw)
                cpw = cw - cx;
            if (cpw <= 0)
                continue;

            /* safety: dst must be within buffer */
            if (dst < canvas->data || dst + cpw * ch > buf_end)
                continue;

            memcpy(dst, src, (size_t)cpw * ch);
        }

        /* ── BOTTOM bleed ── */
        for (int32_t b = 1; b <= bp; b++)
        {
            int32_t ty = cy + ch2 - 1 + b;
            if (ty >= cht)
                break;

            const uint8_t *src = canvas->data + (size_t)(cy + ch2 - 1) * str + (size_t)cx * ch;
            uint8_t *dst = canvas->data + (size_t)ty * str + (size_t)cx * ch;

            int32_t cpw = cw2;
            if (cx + cpw > cw)
                cpw = cw - cx;
            if (cpw <= 0)
                continue;

            if (dst < canvas->data || dst + cpw * ch > buf_end)
                continue;

            memcpy(dst, src, (size_t)cpw * ch);
        }

        /* ── LEFT + RIGHT bleed, row by row ── */
        int32_t ry0 = cy - bp;
        int32_t ry1 = cy + ch2 + bp;
        if (ry0 < 0)
            ry0 = 0;
        if (ry1 > cht)
            ry1 = cht;

        for (int32_t py = ry0; py < ry1; py++)
        {
            uint8_t *row = canvas->data + (size_t)py * str;

            /* LEFT */
            if (cx >= 0 && cx < cw)
            {
                const uint8_t *sl = row + (size_t)cx * ch;
                for (int32_t b = 1; b <= bp; b++)
                {
                    int32_t tx = cx - b;
                    if (tx < 0)
                        break;
                    uint8_t *d = row + (size_t)tx * ch;
                    if (d < canvas->data || d + ch > buf_end)
                        break;
                    d[0] = sl[0];
                    d[1] = sl[1];
                    d[2] = sl[2];
                }
            }

            /* RIGHT */
            int32_t rx = cx + cw2 - 1;
            if (rx >= 0 && rx < cw)
            {
                const uint8_t *sr = row + (size_t)rx * ch;
                for (int32_t b = 1; b <= bp; b++)
                {
                    int32_t tx = rx + b;
                    if (tx >= cw)
                        break;
                    uint8_t *d = row + (size_t)tx * ch;
                    if (d < canvas->data || d + ch > buf_end)
                        break;
                    d[0] = sr[0];
                    d[1] = sr[1];
                    d[2] = sr[2];
                }
            }
        }
    }

    return IMG_SUCCESS;
}

// // src/pipeline/bleed.c
// //
// // SEGV FIX: bleed writes were going outside canvas bounds.
// //
// // Root cause: the y_start/y_end calculation used unchecked
// // arithmetic that could underflow (y < bleed_px gives wrap-around
// // on uint32_t) or overflow past canvas height.
// //
// // All coordinates are now computed in int32_t, clamped to
// // [0, canvas_dim) before any pixel write. Same fix applied
// // to x-axis bleed. No write ever touches memory outside the
// // canvas buffer.

// #define _GNU_SOURCE

// #include "core/buffer.h"
// #include "pipeline/layout.h"
// #include "api/v1/img_error.h"
// #include <string.h>

// img_result_t img_apply_bleed(
//     img_buffer_t *canvas,
//     const img_layout_t *layout,
//     uint32_t bleed_px)
// {
//     if (!canvas || !layout || bleed_px == 0)
//         return IMG_SUCCESS;

//     const uint32_t ch = 3;
//     const int32_t cw = (int32_t)canvas->width;
//     const int32_t ch2 = (int32_t)canvas->height;
//     const uint32_t str = canvas->stride;

//     for (uint32_t i = 0; i < layout->count; i++)
//     {
//         const img_cell_t *c = &layout->cells[i];

//         const int32_t x = (int32_t)c->x;
//         const int32_t y = (int32_t)c->y;
//         const int32_t w = (int32_t)c->w;
//         const int32_t h = (int32_t)c->h;
//         const int32_t bp = (int32_t)bleed_px;

//         /* ── TOP: copy first photo row upward ── */
//         for (int32_t b = 1; b <= bp; b++)
//         {
//             int32_t ty = y - b;
//             if (ty < 0)
//                 break; /* hit canvas top edge */

//             /* source: first row of photo at y */
//             const uint8_t *src = canvas->data + (size_t)y * str + (size_t)x * ch;
//             uint8_t *dst = canvas->data + (size_t)ty * str + (size_t)x * ch;

//             /* clamp copy width to canvas right edge */
//             int32_t copy_w = w;
//             if (x + copy_w > cw)
//                 copy_w = cw - x;
//             if (copy_w <= 0)
//                 continue;

//             memcpy(dst, src, (size_t)copy_w * ch);
//         }

//         /* ── BOTTOM: copy last photo row downward ── */
//         for (int32_t b = 1; b <= bp; b++)
//         {
//             int32_t ty = y + h - 1 + b;
//             if (ty >= ch2)
//                 break; /* hit canvas bottom edge */

//             const uint8_t *src = canvas->data + (size_t)(y + h - 1) * str + (size_t)x * ch;
//             uint8_t *dst = canvas->data + (size_t)ty * str + (size_t)x * ch;

//             int32_t copy_w = w;
//             if (x + copy_w > cw)
//                 copy_w = cw - x;
//             if (copy_w <= 0)
//                 continue;

//             memcpy(dst, src, (size_t)copy_w * ch);
//         }

//         /* ── LEFT + RIGHT: pixel-by-pixel per row ── */

//         /* row range: include bleed rows above and below photo */
//         int32_t ry_start = y - bp;
//         int32_t ry_end = y + h + bp;

//         /* clamp to canvas */
//         if (ry_start < 0)
//             ry_start = 0;
//         if (ry_end > ch2)
//             ry_end = ch2;

//         for (int32_t py = ry_start; py < ry_end; py++)
//         {
//             uint8_t *row = canvas->data + (size_t)py * str;

//             /* ── LEFT bleed: extend leftmost photo pixel leftward ── */
//             if (x >= 0 && x < cw)
//             {
//                 const uint8_t *src_l = row + (size_t)x * ch;

//                 for (int32_t b = 1; b <= bp; b++)
//                 {
//                     int32_t tx = x - b;
//                     if (tx < 0)
//                         break; /* hit canvas left edge */

//                     uint8_t *dst = row + (size_t)tx * ch;
//                     dst[0] = src_l[0];
//                     dst[1] = src_l[1];
//                     dst[2] = src_l[2];
//                 }
//             }

//             /* ── RIGHT bleed: extend rightmost photo pixel rightward ── */
//             int32_t rx = x + w - 1;
//             if (rx >= 0 && rx < cw)
//             {
//                 const uint8_t *src_r = row + (size_t)rx * ch;

//                 for (int32_t b = 1; b <= bp; b++)
//                 {
//                     int32_t tx = rx + b;
//                     if (tx >= cw)
//                         break; /* hit canvas right edge */

//                     uint8_t *dst = row + (size_t)tx * ch;
//                     dst[0] = src_r[0];
//                     dst[1] = src_r[1];
//                     dst[2] = src_r[2];
//                 }
//             }
//         }
//     }

//     return IMG_SUCCESS;
// }

// // // src/pipeline/bleed.c

// // // Bleed: extend edge pixels outward past photo boundary.
// // // Ported from bleed_plugin.c, fully renamed, no old types.

// // #include "core/buffer.h"
// // #include "pipeline/layout.h"
// // #include "api/v1/img_error.h"
// // #include <string.h>

// // /*
// //  * img_apply_bleed()
// //  *
// //  * For each photo cell, extend the edge pixels outward by bleed_px.
// //  * This prevents white borders when the print sheet is cut.
// //  *
// //  * Operates in-place on the canvas buffer.
// //  * No allocation. Pure pixel copy.
// //  */
// // img_result_t img_apply_bleed(
// //     img_buffer_t *canvas,
// //     const img_layout_t *layout,
// //     uint32_t bleed_px)
// // {
// //     if (!canvas || !layout || bleed_px == 0)
// //         return IMG_SUCCESS;

// //     const uint32_t ch = 3;
// //     const uint32_t cw = canvas->width;
// //     const uint32_t ch_px = canvas->height;

// //     for (uint32_t i = 0; i < layout->count; i++)
// //     {
// //         const img_cell_t *c = &layout->cells[i];
// //         const uint32_t x = c->x, y = c->y;
// //         const uint32_t w = c->w, h = c->h;
// //         const uint32_t row_bytes = w * ch;

// //         /* TOP — copy first row upward */
// //         for (uint32_t b = 1; b <= bleed_px; b++)
// //         {
// //             if (y < b)
// //                 break;
// //             memcpy(canvas->data + (size_t)(y - b) * canvas->stride + x * ch,
// //                    canvas->data + (size_t)y * canvas->stride + x * ch,
// //                    row_bytes);
// //         }

// //         /* BOTTOM — copy last row downward */
// //         for (uint32_t b = 1; b <= bleed_px; b++)
// //         {
// //             uint32_t ty = y + h - 1 + b;
// //             if (ty >= ch_px)
// //                 break;
// //             memcpy(canvas->data + (size_t)ty * canvas->stride + x * ch,
// //                    canvas->data + (size_t)(y + h - 1) * canvas->stride + x * ch,
// //                    row_bytes);
// //         }

// //         /* LEFT + RIGHT — row by row */
// //         uint32_t y_start = (y >= bleed_px) ? y - bleed_px : 0;
// //         uint32_t y_end = (y + h + bleed_px < ch_px) ? y + h + bleed_px : ch_px;

// //         for (uint32_t py = y_start; py < y_end; py++)
// //         {
// //             uint8_t *row = canvas->data + (size_t)py * canvas->stride;

// //             /* LEFT */
// //             const uint8_t *src_l = row + x * ch;
// //             for (uint32_t b = 1; b <= bleed_px; b++)
// //             {
// //                 if (x < b)
// //                     break;
// //                 uint8_t *dst = row + (x - b) * ch;
// //                 dst[0] = src_l[0];
// //                 dst[1] = src_l[1];
// //                 dst[2] = src_l[2];
// //             }

// //             /* RIGHT */
// //             const uint8_t *src_r = row + (x + w - 1) * ch;
// //             for (uint32_t b = 1; b <= bleed_px; b++)
// //             {
// //                 uint32_t tx = x + w - 1 + b;
// //                 if (tx >= cw)
// //                     break;
// //                 uint8_t *dst = row + tx * ch;
// //                 dst[0] = src_r[0];
// //                 dst[1] = src_r[1];
// //                 dst[2] = src_r[2];
// //             }
// //         }
// //     }

// //     return IMG_SUCCESS;
// // }
