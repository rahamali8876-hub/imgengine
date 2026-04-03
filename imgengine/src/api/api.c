// // src/api/api.c

#include "api/v1/img_api.h"
#include "include/core/context_internal.h"
#include "memory/slab.h"
#include "plugins/plugin_internal.h"

#include <stdlib.h>

// ==========================================
// INIT
// ==========================================
img_engine_t *img_api_init(uint32_t workers)
{
    img_engine_t *engine = malloc(sizeof(img_engine_t));
    if (!engine)
        return NULL;

    engine->worker_count = workers;

    engine->global_pool = img_slab_create(
        64 * 1024 * 1024, // 64MB
        64 * 1024         // 64KB blocks
    );

    img_plugins_init_all();

    return engine;
}

// ==========================================
// FAST PROCESS (Minimal Working Path)
// ==========================================
int img_api_process_fast(
    img_engine_t *engine,
    uint8_t *input,
    size_t input_size,
    uint8_t **output,
    size_t *output_size)
{
    (void)engine;
    (void)input;
    (void)input_size;

    // 🔥 Placeholder pipeline (you will wire real later)
    *output = input;
    *output_size = input_size;

    return 0;
}

// ==========================================
// SHUTDOWN
// ==========================================
void img_api_shutdown(img_engine_t *engine)
{
    if (!engine)
        return;

    img_slab_destroy(engine->global_pool);

    free(engine);
}

// #include <stdlib.h>
// #include "api/v1/img_api.h"

// #include "core/context_internal.h"
// #include "io/decoder/decoder_entry.h"
// #include "io/encoder/encoder_entry.h"

// #include "pipeline/pipeline.h"
// #include "pipeline/jump_table.h"

// #include "plugins/plugin_internal.h"

// #include "memory/slab.h"
// #include "memory/arena.h"

// #include "arch/cpu_caps.h"

// // forward
// void img_engine_init(img_engine_t *engine);

// // ============================================
// // INIT
// // ============================================

// img_engine_t *img_api_init(void)
// {
//     img_engine_t *engine = malloc(sizeof(img_engine_t));
//     if (!engine)
//         return NULL;

//     engine->worker_count = 1; // 🔥 start simple

//     // 🔥 global slab
//     engine->global_pool = img_slab_create(
//         64 * 1024 * 1024, // 64MB
//         1 * 1024 * 1024); // 1MB blocks

//     img_engine_init(engine);

//     return engine;
// }

// static void build_default_pipeline(img_pipeline_t *p)
// {
//     p->op_count = 0;

//     // 🔥 example pipeline
//     // resize → grayscale

//     static resize_params_t resize = {
//         .target_w = 512,
//         .target_h = 512};

//     p->ops[p->op_count] = OP_RESIZE;
//     p->params[p->op_count++] = &resize;

//     p->ops[p->op_count] = OP_GRAYSCALE;
//     p->params[p->op_count++] = NULL;
// }

// int img_api_process_fast(
//     img_engine_t *engine,
//     uint8_t *input,
//     size_t input_size,
//     uint8_t **output,
//     size_t *output_size)
// {
//     if (!engine || !input || !output)
//         return -1;

//     // 🔥 THREAD CONTEXT (single-thread for now)
//     img_ctx_t ctx = {0};

//     ctx.thread_id = 0;
//     ctx.local_pool = engine->global_pool;
//     ctx.scratch_arena = NULL;
//     ctx.caps = engine->caps;

//     // =====================================
//     // 1. DECODE
//     // =====================================
//     img_buffer_t buf;

//     if (img_decode_to_buffer(&ctx, input, input_size, &buf) != 0)
//         return -1;

//     // =====================================
//     // 2. PIPELINE
//     // =====================================
//     img_pipeline_t pipeline;
//     build_default_pipeline(&pipeline);

//     img_pipeline_execute(&ctx, &pipeline, &buf);

//     // =====================================
//     // 3. ENCODE
//     // =====================================
//     if (img_encode_from_buffer(&ctx, &buf, output, output_size) != 0)
//         return -1;

//     return 0;
// }

// void img_api_shutdown(img_engine_t *engine)
// {
//     if (!engine)
//         return;

//     img_slab_destroy(engine->global_pool);

//     free(engine);
// }