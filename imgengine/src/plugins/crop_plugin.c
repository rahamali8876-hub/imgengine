// // src/plugins/crop_plugin.c

// #include "imgengine/plugins/plugin.h"
// #include "imgengine/layout.h"

// // draw simple L-shaped marks
// static void draw_mark(img_t *img, int x, int y, int len, int thickness)
// {
//     for (int t = 0; t < thickness; t++)
//     {
//         for (int i = 0; i < len; i++)
//         {
//             // horizontal
//             int idx1 = ((y + t) * img->width + (x + i)) * 3;
//             // vertical
//             int idx2 = ((y + i) * img->width + (x + t)) * 3;

//             img->data[idx1 + 0] = 0;
//             img->data[idx1 + 1] = 0;
//             img->data[idx1 + 2] = 0;

//             img->data[idx2 + 0] = 0;
//             img->data[idx2 + 1] = 0;
//             img->data[idx2 + 2] = 0;
//         }
//     }
// }

// static int crop_process(img_ctx_t *ctx,
//                         img_t *canvas,
//                         const img_job_t *job)
// {
//     if (job->crop_mark_px <= 0)
//         return 1;

//     img_layout_info_t *layout = layout_get(ctx);

//     for (int i = 0; i < layout->count; i++)
//     {
//         img_cell_t c = layout->cells[i];

//         int len = job->crop_mark_px;
//         int th = job->crop_thickness;

//         // 🔥 4 corners
//         draw_mark(canvas, c.x - len, c.y - len, len, th); // TL
//         draw_mark(canvas, c.x + c.w, c.y - len, len, th); // TR
//         draw_mark(canvas, c.x - len, c.y + c.h, len, th); // BL
//         draw_mark(canvas, c.x + c.w, c.y + c.h, len, th); // BR
//     }

//     return 1;
// }

// static img_plugin_t plugin = {
//     .name = "crop_marks",
//     .process = crop_process};

// img_plugin_t *get_crop_plugin()
// {
//     return &plugin;
// }

#include "imgengine/plugins/plugin.h"
#include "imgengine/layout.h"

static void draw_line_h(img_t *img, int x, int y, int len, int th)
{
    for (int t = 0; t < th; t++)
        for (int i = 0; i < len; i++)
        {
            int xx = x + i;
            int yy = y + t;

            if (xx < 0 || xx >= img->width || yy < 0 || yy >= img->height)
                continue;

            int idx = (yy * img->width + xx) * 3;
            img->data[idx + 0] = 0;
            img->data[idx + 1] = 0;
            img->data[idx + 2] = 0;
        }
}

static void draw_line_v(img_t *img, int x, int y, int len, int th)
{
    for (int t = 0; t < th; t++)
        for (int i = 0; i < len; i++)
        {
            int xx = x + t;
            int yy = y + i;

            if (xx < 0 || xx >= img->width || yy < 0 || yy >= img->height)
                continue;

            int idx = (yy * img->width + xx) * 3;
            img->data[idx + 0] = 0;
            img->data[idx + 1] = 0;
            img->data[idx + 2] = 0;
        }
}

static int crop_process(img_ctx_t *ctx,
                        img_t *canvas,
                        const img_job_t *job)
{
    if (job->crop_mark_px <= 0)
        return 1;

    img_layout_info_t *layout = layout_get(ctx);

    int bleed = job->bleed_px;
    int offset = job->crop_offset_px;

    for (int i = 0; i < layout->count; i++)
    {
        img_cell_t c = layout->cells[i];

        int len = job->crop_mark_px;
        int th = job->crop_thickness;

        // 🔥 final offset = bleed + gap
        int d = bleed + offset;

        // ===== TOP LEFT =====
        draw_line_h(canvas, c.x - d - len, c.y - d, len, th);
        draw_line_v(canvas, c.x - d, c.y - d - len, len, th);

        // ===== TOP RIGHT =====
        draw_line_h(canvas, c.x + c.w + d, c.y - d, len, th);
        draw_line_v(canvas, c.x + c.w + d, c.y - d - len, len, th);

        // ===== BOTTOM LEFT =====
        draw_line_h(canvas, c.x - d - len, c.y + c.h + d, len, th);
        draw_line_v(canvas, c.x - d, c.y + c.h + d, len, th);

        // ===== BOTTOM RIGHT =====
        draw_line_h(canvas, c.x + c.w + d, c.y + c.h + d, len, th);
        draw_line_v(canvas, c.x + c.w + d, c.y + c.h + d, len, th);
    }

    return 1;
}

static img_plugin_t plugin = {
    .name = "crop_marks_iso",
    .process = crop_process};

img_plugin_t *get_crop_plugin()
{
    return &plugin;
}