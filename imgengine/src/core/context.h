/* src/core/context.h */
#ifndef IMGENGINE_CORE_CONTEXT_H
#define IMGENGINE_CORE_CONTEXT_H

// #include <stdint.h>
#include "memory/slab.h"
#include "arch/cpu_caps.h"

// typedef struct img_ctx
// {
//     uint32_t worker_id;
//     img_slab_pool_t *pool;
//     cpu_arch_t cpu_caps;
// } img_ctx_t;

#include <turbojpeg.h>

typedef struct img_ctx
{
    uint32_t worker_id;
    img_slab_pool_t *pool;
    cpu_arch_t cpu_caps;

    // 🔥 NEW (per-worker JPEG engines)
    tjhandle tj_decoder;
    tjhandle tj_encoder;

} img_ctx_t;
#endif