// ./src/runtime/worker.c

// ================================================================
// FIX 2 OF 3
// FILE: src/runtime/worker.c
// CHANGE: Accept scheduler + ctx in init. Set both BEFORE
//         pthread_create. Null-guard steal as defense-in-depth.
// ================================================================

// src/runtime/worker.c

#include "runtime/worker.h"
#include "runtime/task.h"
#include "runtime/scheduler.h"
#include "hot/pipeline_exec.h"
#include "runtime/affinity.h"
#include "observability/binlog.h"

#include <stdlib.h>
#include <immintrin.h>

extern void img_binlog_set_cpu(uint32_t cpu);

_Static_assert(
    __builtin_offsetof(img_task_t, pipeline) ==
        __builtin_offsetof(img_task_t, pipeline),
    "img_task_t layout must not change without updating worker.c");

static void *worker_loop(void *arg)
{
    img_worker_t *w = (img_worker_t *)arg;

    img_binlog_set_cpu(w->id);

    while (__builtin_expect(w->running, 1))
    {

        img_task_t *task = (img_task_t *)img_queue_pop(w->queue);

        /*
         * STEAL: only attempt if scheduler is wired.
         * w->scheduler is set before pthread_create (see init below)
         * so this check is purely defensive — should never be NULL.
         */
        if (!task && w->scheduler)
            task = img_scheduler_steal(w->scheduler, w->id);

        if (!task)
        {
            _mm_pause();
            continue;
        }

        /*
         * Guard: ctx must be set. If NULL, skip task — don't crash.
         * In normal operation ctx is always set before thread starts.
         */
        if (__builtin_expect(!w->ctx, 0))
        {
            task->status = -1;
            continue;
        }

        img_pipeline_execute_hot(
            w->ctx,
            task->pipeline,
            task->buffer);

        task->status = 0;
    }

    return NULL;
}

int img_worker_init(
    img_worker_t *w,
    uint32_t id,
    struct img_scheduler *scheduler,
    struct img_ctx *ctx)
{
    if (!w)
        return -1;

    w->id = id;
    w->running = 1;
    w->scheduler = scheduler; /* set BEFORE pthread_create */
    w->ctx = ctx;             /* set BEFORE pthread_create */

    w->queue = img_queue_create(10);
    if (!w->queue)
        return -1;

    img_pin_thread_to_core(id);

    pthread_create(&w->thread, NULL, worker_loop, w);

    return 0;
}

void img_worker_stop(img_worker_t *w)
{
    if (w)
        w->running = 0;
}

void img_worker_join(img_worker_t *w)
{
    if (w)
        pthread_join(w->thread, NULL);
}
