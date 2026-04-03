/* include/core/context_internal.h */

#ifndef IMGENGINE_CONTEXT_INTERNAL_H
#define IMGENGINE_CONTEXT_INTERNAL_H

#include <stdint.h>
#include <stddef.h>

#include "api/v1/img_core.h"
#include "arch/cpu_caps.h"

#include "memory/slab.h"
#include "memory/arena.h"

// Forward declaration (owned by runtime/worker.h)
struct img_worker_s;

// ================= ENGINE =================
struct img_engine_s
{
    // 🔥 HOT PATH
    uint32_t worker_count;
    struct img_worker_s *workers;

    // 🔥 SEMI-HOT
    cpu_caps_t caps;

    // ❄️ COLD
    img_slab_pool_t *global_pool;

    // 🔮 FUTURE EXTENSION
    void *user_data;
};

// ================= THREAD CONTEXT =================
typedef struct __attribute__((aligned(64))) img_ctx
{
    uint32_t thread_id;

    img_slab_pool_t *local_pool;
    img_arena_t *scratch_arena;

    cpu_caps_t caps;

} img_ctx_t;

#endif
