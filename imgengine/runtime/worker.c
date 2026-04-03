/* runtime/worker.c */

#include "runtime/worker.h"
#include "pipeline/engine.h"
#include "observability/metrics.h"
#include "observability/profiler.h"
#include "runtime/worker.h"
#include "runtime/affinity.h"
#include "memory/numa.h"

static void *worker_loop(void *arg)
{
    img_worker_t *w = (img_worker_t *)arg;

    while (w->running)
    {
        img_task_t *task = NULL;

        // 🔥 LOCK-FREE POP
        if (!img_queue_pop(w->queue, (void **)&task))
            continue;

        uint64_t start = img_profiler_now();

        // 🔥 EXECUTE PIPELINE
        task->status = img_pipeline_execute(
            &w->ctx,
            task->input,
            task->input_size,
            task->pipeline,
            &task->output,
            &task->output_size);

        uint64_t end = img_profiler_now();

        img_metrics_inc(&g_metrics.total_requests);
        img_metrics_observe_latency(end - start);
    }

    return NULL;
}

int img_worker_init(img_worker_t *w, uint32_t id)
{
    w->id = id;
    w->running = 1;

    // 🔥 NUMA-aware allocation
    int node = img_numa_get_node(id);

    w->slab = img_slab_create_on_node(node);
    w->arena = img_arena_create_on_node(node);

    w->queue = img_queue_create(1024);

    // 🔥 Setup context
    w->ctx.thread_id = id;
    w->ctx.local_pool = w->slab;
    w->ctx.scratch_arena = w->arena;

    // 🔥 Pin thread
    pthread_create(&w->thread, NULL, worker_loop, w);
    img_set_thread_affinity(w->thread, id);

    return 0;
}

// #include "runtime/worker.h"
// #include "runtime/queue_mpmc.h"
// #include "runtime/affinity.h"
// #include "pipeline/jump_table.h"
// #include "security/sandbox.h"
// #include "runtime/task.h"
// #include "io/decoder/decoder_entry.h"
// #include "io/encoder/encoder_entry.h"
// #include "hot/pipeline_exec.h"

// #include <stdlib.h>

// void *img_worker_loop(void *arg)
// {
//     img_worker_t *self = (img_worker_t *)arg;

//     img_pin_thread_to_core(self->cpu_id);
//     img_security_enter_sandbox();
//     img_pipeline_bind_thread();

//     while (self->active)
//     {
//         img_task_t *task = img_mpmc_pop(self->task_queue);

//         if (!task)
//         {
//             __builtin_ia32_pause();
//             continue;
//         }

//         img_buffer_t buf;

//         // 🔥 1. DECODE
//         if (img_decode_to_buffer(self->ctx,
//                                  task->input_data,
//                                  task->input_size,
//                                  &buf) != 0)
//         {
//             free(task);
//             continue;
//         }

//         // 🔥 2. PROCESS (SIMD PIPELINE)
//         // img_execute_pipeline(self->ctx,
//         //                      &buf,
//         //                      task->ops,
//         //                      task->params,
//         //                      task->op_count);

//         // 🔥 3. ENCODE
//         img_encode_from_buffer(self->ctx,
//                                &buf,
//                                &task->output_data,
//                                &task->output_size);

//         // 🔥 4. FREE SLAB (IMPORTANT)
//         img_slab_free(self->ctx->pool, buf.data);

//         // (later: push result to output queue)

//         free(task);
//     }

//     return NULL;
// }
