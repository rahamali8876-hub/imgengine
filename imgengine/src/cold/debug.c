
// src/cold/debug.c

// src/cold/debug.c

#include "cold/debug.h"
#include <stdio.h>

// These must match your cpu_caps.h
#define CPU_CAP_AVX2 (1 << 0)
#define CPU_CAP_AVX512 (1 << 1)
#define CPU_CAP_NEON (1 << 2)

void img_debug_dump_ctx(const img_ctx_t *ctx)
{
    if (!ctx)
        return;

    printf("[DEBUG] Thread: %u Pool: %p Caps: 0x%x\n",
           ctx->thread_id,
           (void *)ctx->local_pool,
           (unsigned int)ctx->caps);

#ifdef ARCH_X86_64
    printf("[DEBUG] AVX2: %d AVX512: %d\n",
           (ctx->caps & CPU_CAP_AVX2) != 0,
           (ctx->caps & CPU_CAP_AVX512) != 0);
#elif defined(ARCH_AARCH64)
    printf("[DEBUG] NEON: %d\n",
           (ctx->caps & CPU_CAP_NEON) != 0);
#endif
}

// #include "cold/debug.h"
// #include <stdio.h>

// void img_debug_dump_ctx(const img_ctx_t *ctx)
// {
//     if (!ctx)
//         return;

//     printf("[DEBUG] Thread: %u Pool: %p Caps: %u\n",
//            ctx->thread_id,
//            (void *)ctx->local_pool,
//            (unsigned int)ctx->caps.flags);

// #ifdef ARCH_X86_64
//     printf("[DEBUG] AVX2: %d AVX512: %d\n",
//            ctx->caps.avx2,
//            ctx->caps.avx512);
// #elif defined(ARCH_AARCH64)
//     printf("[DEBUG] NEON: %d\n",
//            ctx->caps.neon);
// #endif
// }

// #include "cold/debug.h"
// #include <stdio.h>

// void img_debug_dump_ctx(const img_ctx_t *ctx)
// {
//     if (!ctx)
//         return;

//     printf("[DEBUG] Thread: %u Pool: %p Caps: %u\n",
//            ctx->thread_id,
//            (void *)ctx->local_pool,
//            ctx->caps);
//     printf("[DEBUG] AVX2: %d AVX512: %d\n",
//            ctx->caps.avx2,
//            ctx->caps.avx512);
// }

// #include "cold/debug.h"
// #include <stdio.h>

// void img_debug_dump_ctx(const img_ctx_t *ctx)
// {
//     if (!ctx)
//         return;

//     printf("[DEBUG] Worker: %u Pool: %p Caps: %u\n",
//            ctx->worker_id, // ✅ FIXED
//            (void *)ctx->pool,
//            ctx->cpu_caps);
// }