// // src/plugins/bleed_plugin.c

#include "imgengine/plugins/plugin.h"
#include "imgengine/layout.h"

// 🔥 extend edges (real bleed)
static void apply_bleed(img_t *img, int x, int y, int w, int h, int bleed)
{
    int channels = 3;

    // TOP
    for (int by = 1; by <= bleed; by++)
    {
        for (int i = 0; i < w; i++)
        {
            int src = ((y)*img->width + (x + i)) * channels;
            int dst = ((y - by) * img->width + (x + i)) * channels;

            for (int c = 0; c < channels; c++)
                img->data[dst + c] = img->data[src + c];
        }
    }

    // BOTTOM
    for (int by = 1; by <= bleed; by++)
    {
        for (int i = 0; i < w; i++)
        {
            int src = ((y + h - 1) * img->width + (x + i)) * channels;
            int dst = ((y + h - 1 + by) * img->width + (x + i)) * channels;

            for (int c = 0; c < channels; c++)
                img->data[dst + c] = img->data[src + c];
        }
    }

    // LEFT + RIGHT
    for (int j = -bleed; j < h + bleed; j++)
    {
        for (int bx = 1; bx <= bleed; bx++)
        {
            int yy = y + j;

            if (yy < 0 || yy >= img->height)
                continue;

            // LEFT
            int srcL = (yy * img->width + x) * channels;
            int dstL = (yy * img->width + (x - bx)) * channels;

            // RIGHT
            int srcR = (yy * img->width + (x + w - 1)) * channels;
            int dstR = (yy * img->width + (x + w - 1 + bx)) * channels;

            if (x - bx >= 0)
                for (int c = 0; c < channels; c++)
                    img->data[dstL + c] = img->data[srcL + c];

            if (x + w - 1 + bx < img->width)
                for (int c = 0; c < channels; c++)
                    img->data[dstR + c] = img->data[srcR + c];
        }
    }
}

static int bleed_process(img_ctx_t *ctx,
                         img_t *canvas,
                         const img_job_t *job)
{
    if (job->bleed_px <= 0)
        return 1;

    img_layout_info_t *layout = layout_get(ctx);

    for (int i = 0; i < layout->count; i++)
    {
        img_cell_t c = layout->cells[i];

        apply_bleed(canvas,
                    c.x, c.y,
                    c.w, c.h,
                    job->bleed_px);
    }

    return 1;
}

static img_plugin_t plugin = {
    .name = "bleed",
    .process = bleed_process};

img_plugin_t *get_bleed_plugin()
{
    return &plugin;
}

// #include "imgengine/plugins/plugin.h"
// #include "imgengine/layout.h"

// // clamp helper
// static inline int clamp(int v, int min, int max)
// {
//     return v < min ? min : (v > max ? max : v);
// }

// // extend edge pixels
// static void apply_bleed(img_t *img,
//                         int x, int y,
//                         int w, int h,
//                         int bleed)
// {
//     int W = img->width;
//     int H = img->height;

//     // TOP BLEED
//     for (int by = 1; by <= bleed; by++)
//     {
//         int src_y = y;
//         int dst_y = y - by;
//         if (dst_y < 0)
//             continue;

//         for (int ix = 0; ix < w; ix++)
//         {
//             int px = clamp(x + ix, 0, W - 1);

//             int src = (src_y * W + px) * 3;
//             int dst = (dst_y * W + px) * 3;

//             img->data[dst + 0] = img->data[src + 0];
//             img->data[dst + 1] = img->data[src + 1];
//             img->data[dst + 2] = img->data[src + 2];
//         }
//     }

//     // BOTTOM BLEED
//     for (int by = 1; by <= bleed; by++)
//     {
//         int src_y = y + h - 1;
//         int dst_y = y + h - 1 + by;
//         if (dst_y >= H)
//             continue;

//         for (int ix = 0; ix < w; ix++)
//         {
//             int px = clamp(x + ix, 0, W - 1);

//             int src = (src_y * W + px) * 3;
//             int dst = (dst_y * W + px) * 3;

//             img->data[dst + 0] = img->data[src + 0];
//             img->data[dst + 1] = img->data[src + 1];
//             img->data[dst + 2] = img->data[src + 2];
//         }
//     }

//     // LEFT BLEED
//     for (int bx = 1; bx <= bleed; bx++)
//     {
//         int src_x = x;
//         int dst_x = x - bx;
//         if (dst_x < 0)
//             continue;

//         for (int iy = 0; iy < h; iy++)
//         {
//             int py = clamp(y + iy, 0, H - 1);

//             int src = (py * W + src_x) * 3;
//             int dst = (py * W + dst_x) * 3;

//             img->data[dst + 0] = img->data[src + 0];
//             img->data[dst + 1] = img->data[src + 1];
//             img->data[dst + 2] = img->data[src + 2];
//         }
//     }

//     // RIGHT BLEED
//     for (int bx = 1; bx <= bleed; bx++)
//     {
//         int src_x = x + w - 1;
//         int dst_x = x + w - 1 + bx;
//         if (dst_x >= W)
//             continue;

//         for (int iy = 0; iy < h; iy++)
//         {
//             int py = clamp(y + iy, 0, H - 1);

//             int src = (py * W + src_x) * 3;
//             int dst = (py * W + dst_x) * 3;

//             img->data[dst + 0] = img->data[src + 0];
//             img->data[dst + 1] = img->data[src + 1];
//             img->data[dst + 2] = img->data[src + 2];
//         }
//     }
// }

// static int bleed_process(img_ctx_t *ctx,
//                          img_t *canvas,
//                          const img_job_t *job)
// {
//     if (job->bleed_px <= 0)
//         return 1;

//     img_layout_info_t *layout = layout_get(ctx);

//     for (int i = 0; i < layout->count; i++)
//     {
//         img_cell_t c = layout->cells[i];

//         apply_bleed(canvas,
//                     c.x, c.y,
//                     c.w, c.h,
//                     job->bleed_px);
//     }

//     return 1;
// }

// static img_plugin_t plugin = {
//     .name = "bleed",
//     .process = bleed_process};

// img_plugin_t *get_bleed_plugin()
// {
//     return &plugin;
// }
