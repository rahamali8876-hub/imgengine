/* runtime/worker.h */
#include "runtime/queue_mpmc.h"

#ifndef IMGENGINE_RUNTIME_WORKER_H
#define IMGENGINE_RUNTIME_WORKER_H

#include <pthread.h>
#include <stdbool.h>
#include "core/context.h"
#include "runtime/queue_spsc.h"

typedef struct
{
    pthread_t thread;
    uint32_t cpu_id;
    img_ctx_t *ctx;
    // img_queue_t *task_queue;
    volatile bool active;
    // int cpu_id;
    // int active;

    img_mpmc_queue_t *task_queue; // 🔥 CHANGE THIS

} img_worker_t;

/**
 * Worker main loop
 */
void *img_worker_loop(void *arg);

#endif