// observability/metrics.c 

#include "observability/metrics.h"
#include <stdio.h>

img_metrics_t g_metrics = {0};

void img_metrics_init(void)
{
    __atomic_store_n(&g_metrics.min_latency, UINT64_MAX, __ATOMIC_RELAXED);
}

void img_metrics_inc(uint64_t *counter)
{
    __atomic_fetch_add(counter, 1, __ATOMIC_RELAXED);
}

void img_metrics_observe_latency(uint64_t cycles)
{
    __atomic_fetch_add(&g_metrics.total_cycles, cycles, __ATOMIC_RELAXED);

    uint64_t old_min = g_metrics.min_latency;
    if (cycles < old_min)
        __atomic_store_n(&g_metrics.min_latency, cycles, __ATOMIC_RELAXED);

    uint64_t old_max = g_metrics.max_latency;
    if (cycles > old_max)
        __atomic_store_n(&g_metrics.max_latency, cycles, __ATOMIC_RELAXED);
}

void img_metrics_export_prometheus(char *buf, size_t size)
{
    uint64_t total = g_metrics.total_requests;
    uint64_t avg = total ? (g_metrics.total_cycles / total) : 0;

    snprintf(buf, size,
             "img_latency_avg %lu\n"
             "img_latency_max %lu\n"
             "img_requests_total %lu\n"
             "img_backpressure_drops %lu\n",
             avg,
             g_metrics.max_latency,
             total,
             g_metrics.backpressure_drops);
}