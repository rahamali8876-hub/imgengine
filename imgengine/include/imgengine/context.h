// imgengine/context.h

#ifndef IMG_CONTEXT_H
#define IMG_CONTEXT_H

#include "memory_pool.h"

// forward declaration ONLY
struct img_layout_info;

typedef struct img_ctx
{
    mem_pool_t pool;

    struct img_layout_info *layout;

} img_ctx_t;

void img_ctx_init(img_ctx_t *ctx, size_t pool_size);
void img_ctx_destroy(img_ctx_t *ctx);

#endif

// #ifndef IMG_CONTEXT_H
// #define IMG_CONTEXT_H

// #include "memory_pool.h"
// #include "layout.h" // ✅ SAFE NOW

// typedef struct img_ctx
// {
//     mem_pool_t pool;

//     img_layout_info_t *layout; // ✅ FIXED

// } img_ctx_t;

// void img_ctx_init(img_ctx_t *ctx, size_t pool_size);
// void img_ctx_destroy(img_ctx_t *ctx);

// #endif
