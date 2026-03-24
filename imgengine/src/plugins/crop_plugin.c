// src/plugins/crop_plugin.c

#include "imgengine/plugins/plugin.h"

extern void draw_crop_marks(img_t *, int, int, int, int, int, int);

// static int crop_process(img_ctx_t *ctx,
//                         img_t *canvas,
//                         const img_job_t *job)
// {
//     if (job->crop_mark_px <= 0)
//         return 1;

//     // ⚠️ For now global marks skipped
//     // later: store layout positions → draw here

//     return 1;
// }

static int crop_process(img_ctx_t *ctx,
                        img_t *canvas,
                        const img_job_t *job)
{
    (void)ctx;
    (void)canvas;

    if (job->crop_mark_px <= 0)
        return 1;

    return 1;
}

static img_plugin_t plugin = {
    .name = "crop_marks",
    .process = crop_process};

img_plugin_t *get_crop_plugin()
{
    return &plugin;
}