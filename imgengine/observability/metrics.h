// observability/metrics.h

#ifndef IMGENGINE_OBSERVABILITY_METRICS_H
#define IMGENGINE_OBSERVABILITY_METRICS_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint64_t total_requests;
    uint64_t backpressure_drops;
    uint64_t total_cycles;
    uint64_t min_latency;
    uint64_t max_latency;

    uint64_t slab_total;
    uint64_t slab_in_use;

} img_metrics_t;

extern img_metrics_t g_metrics;

void img_metrics_init(void);
void img_metrics_inc(uint64_t *counter);
void img_metrics_observe_latency(uint64_t cycles);
void img_metrics_export_prometheus(char *buffer, size_t size);

#endif