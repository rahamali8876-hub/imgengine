// src/observability/binlog.c

#include "observability/binlog.h"
#include <stdlib.h>
#include <sched.h>
#include "core/time.h"

// ================================
// FAST TIMESTAMP
// ================================
// static inline uint64_t now_ns()
// {
//     struct timespec ts;
//     clock_gettime(CLOCK_MONOTONIC, &ts);
//     return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
// }

static inline uint32_t get_cpu()
{
#ifdef __linux__
    return sched_getcpu();
#else
    return 0;
#endif
}

// ================================
// INIT
// ================================
int img_binlog_init(img_binlog_t *log, uint32_t power)
{
    if (!log)
        return -1;

    uint32_t size = 1u << power;

    log->entries = aligned_alloc(64, sizeof(img_log_entry_t) * size);
    if (!log->entries)
        return -1;

    log->size = size;
    log->mask = size - 1;

    atomic_init(&log->head, 0);
    atomic_init(&log->tail, 0);

    return 0;
}

void img_binlog_destroy(img_binlog_t *log)
{
    if (!log)
        return;

    free(log->entries);
}

// ================================
// WRITE (LOCK-FREE)
// ================================
int img_binlog_write(
    img_binlog_t *log,
    uint32_t event,
    uint64_t arg0,
    uint64_t arg1,
    uint64_t arg2)
{
    uint32_t tail = atomic_load_explicit(&log->tail, memory_order_relaxed);
    uint32_t next = (tail + 1) & log->mask;

    uint32_t head = atomic_load_explicit(&log->head, memory_order_acquire);

    if (next == head)
        return -1; // full → drop

    img_log_entry_t *e = &log->entries[tail];

    // e->timestamp = now_ns();
    e->timestamp = img_now_ns();
    e->event_id = event;
    e->cpu = get_cpu();
    e->arg0 = arg0;
    e->arg1 = arg1;
    e->arg2 = arg2;

    atomic_store_explicit(&log->tail, next, memory_order_release);

    return 0;
}