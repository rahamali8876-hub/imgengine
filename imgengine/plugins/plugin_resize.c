// // plugins/_resize.c
#include "plugins/plugin_internal.h"
#include "plugins/plugin_resize.h"
#include "memory/slab.h"
#include "pipeline/jump_table.h"

// void plugin_resize_single(img_ctx_t *ctx, img_buffer_t *buf, void *params)
// {
//     resize_params_t *p = (resize_params_t *)params;

//     // 🔥 PRECOMPUTE ONCE (NOT INSIDE SIMD LOOP)
//     p->scale_x = (buf->width << 16) / p->target_w;
//     p->scale_y = (buf->height << 16) / p->target_h;

//     // inject src
//     p->src = buf;

//     // 1. Allocate output
//     uint8_t *out_mem = img_slab_alloc(ctx->pool);
//     if (!out_mem)
//         return;

//     img_buffer_t dst = img_buffer_create(
//         out_mem,
//         p->target_w,
//         p->target_h,
//         buf->channels);

//     // 2. Inject source into params
//     p->src = buf;

//     // 3. 🔥 DIRECT EXECUTION (NO ADAPTER, NO SWITCH)
//     g_jump_table[OP_RESIZE](ctx, &dst, params);

//     // 4. Swap buffers
//     img_slab_free(ctx->pool, buf->data);
//     *buf = dst;
// }

static void precompute_indices(resize_params_t *p)
{
    p->x_index = malloc(sizeof(uint32_t) * p->target_w);
    p->y_index = malloc(sizeof(uint32_t) * p->target_h);

    for (uint32_t x = 0; x < p->target_w; x++)
        p->x_index[x] = (x * p->scale_x) >> 16;

    for (uint32_t y = 0; y < p->target_h; y++)
        p->y_index[y] = (y * p->scale_y) >> 16;
}

void plugin_resize_single(img_ctx_t *ctx, img_buffer_t *buf, void *params)
{
    resize_params_t *p = (resize_params_t *)params;

    p->scale_x = (buf->width << 16) / p->target_w;
    p->scale_y = (buf->height << 16) / p->target_h;
    p->src = buf;

    precompute_indices(p);

    // 🔥 TEMP BUFFER (horizontal pass)
    uint8_t *tmp_mem = img_slab_alloc(ctx->pool);

    img_buffer_t tmp = img_buffer_create(
        tmp_mem,
        p->target_w,
        buf->height,
        buf->channels);

    // 🔥 FINAL OUTPUT
    uint8_t *out_mem = img_slab_alloc(ctx->pool);

    img_buffer_t dst = img_buffer_create(
        out_mem,
        p->target_w,
        p->target_h,
        buf->channels);

    // 🔥 EXECUTE PASSES
    g_jump_table[OP_RESIZE_H](ctx, &tmp, params);
    g_jump_table[OP_RESIZE_V](ctx, &dst, params);

    img_slab_free(ctx->pool, buf->data);
    *buf = dst;
}
