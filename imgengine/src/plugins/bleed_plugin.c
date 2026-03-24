// src/plugins/bleed_plugin.c

#include "imgengine/plugins/plugin.h"

// static int bleed_process(img_ctx_t *ctx,
//                          img_t *canvas,
//                          const img_job_t *job)
// {
//     if (job->bleed_px <= 0)
//         return 1;

//     // (simplified — you already built real bleed)
//     // You can reuse apply_bleed here globally if needed

//     return 1;
// }

static int bleed_process(img_ctx_t *ctx,
                         img_t *canvas,
                         const img_job_t *job)
{
    (void)ctx;
    (void)canvas;

    if (job->bleed_px <= 0)
        return 1;

    return 1;
}

static img_plugin_t plugin = {
    .name = "bleed",
    .process = bleed_process};

img_plugin_t *get_bleed_plugin()
{
    return &plugin;
}