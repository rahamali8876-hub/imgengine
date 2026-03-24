// // // src/core/context.c

#include "imgengine/context.h"
#include "imgengine/layout.h"
#include "imgengine/memory_pool.h"

void img_ctx_init(img_ctx_t *ctx, size_t pool_size)
{
    mp_init(&ctx->pool, pool_size);
    ctx->layout = NULL;
}

void img_ctx_destroy(img_ctx_t *ctx)
{
    mp_destroy(&ctx->pool);
}

void layout_store(img_ctx_t *ctx,
                  img_cell_t *cells,
                  int count)
{
    img_layout_info_t *info =
        (img_layout_info_t *)mp_alloc(&ctx->pool, sizeof(img_layout_info_t));

    if (!info)
        return;

    info->cells = cells;
    info->count = count;

    ctx->layout = info;
}

img_layout_info_t *layout_get(img_ctx_t *ctx)
{
    return ctx->layout;
}

// #include "imgengine/context.h"
// #include "imgengine/layout.h"
// #include "imgengine/memory_pool.h"

// void img_ctx_init(img_ctx_t *ctx, size_t pool_size)
// {
//     mp_init(&ctx->pool, pool_size);
//     ctx->layout = NULL;
// }

// void img_ctx_destroy(img_ctx_t *ctx)
// {
//     mp_destroy(&ctx->pool);
// }

// void layout_store(img_ctx_t *ctx,
//                   img_cell_t *cells,
//                   int count)
// {
//     img_layout_info_t *info =
//         (img_layout_info_t *)mp_alloc(&ctx->pool, sizeof(img_layout_info_t));

//     if (!info)
//         return;

//     info->cells = cells;
//     info->count = count;

//     ctx->layout = info;
// }

// img_layout_info_t *layout_get(img_ctx_t *ctx)
// {
//     return ctx->layout;
// }
