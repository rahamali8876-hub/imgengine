/* src/core/context.c */
#include "core/context.h"
#include "memory/slab.h"
#include "core/dispatcher.h"
#include <stdlib.h>

img_ctx_t *img_ctx_init(uint32_t worker_id, size_t slab_size, size_t num_slabs)
{
    img_ctx_t *ctx = malloc(sizeof(img_ctx_t));
    if (!ctx)
        return NULL;

    ctx->worker_id = worker_id;

    // 1. Initialize thread-local Slab Allocator (Zero Fragmentation)
    ctx->pool = img_slab_init(slab_size, num_slabs);

    // 2. Cache CPU capabilities for O(1) dispatching
    ctx->cpu_caps = img_get_cpu_arch();

    return ctx;
}

void img_ctx_destroy(img_ctx_t *ctx)
{
    if (ctx)
    {
        img_slab_destroy(ctx->pool);
        free(ctx);
    }
}
