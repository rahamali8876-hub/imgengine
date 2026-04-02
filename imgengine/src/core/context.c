// src/core/context.c

#include "core/context.h"
#include "memory/slab.h"
#include <stdlib.h>

#define DEFAULT_SLAB_SIZE (64 * 1024 * 1024) // 64MB
#define DEFAULT_BLOCK_SIZE (256 * 1024)      // 256KB per image block

img_ctx_t *img_ctx_create(uint32_t worker_id)
{
    img_ctx_t *ctx = malloc(sizeof(img_ctx_t));
    if (!ctx)
        return NULL;

    ctx->worker_id = worker_id;

    ctx->pool = img_slab_create(DEFAULT_SLAB_SIZE, DEFAULT_BLOCK_SIZE);
    if (!ctx->pool)
    {
        free(ctx);
        return NULL;
    }

    ctx->cpu_caps = img_detect_cpu();

    // 🔥 INIT ONCE
    ctx->tj_decoder = tjInitDecompress();
    ctx->tj_encoder = tjInitCompress();

    if (!ctx->tj_decoder || !ctx->tj_encoder)
    {
        if (ctx->tj_decoder)
            tjDestroy(ctx->tj_decoder);
        if (ctx->tj_encoder)
            tjDestroy(ctx->tj_encoder);
        img_slab_destroy(ctx->pool);
        free(ctx);
        return NULL;
    }

    return ctx;
}

void img_ctx_destroy(img_ctx_t *ctx)
{
    if (!ctx)
        return;

    // 🔥 destroy once
    if (ctx->tj_decoder)
        tjDestroy(ctx->tj_decoder);

    if (ctx->tj_encoder)
        tjDestroy(ctx->tj_encoder);

    img_slab_destroy(ctx->pool);
    free(ctx);
}
