#ifndef IMGENGINE_PIPELINE_GRAPH_H
#define IMGENGINE_PIPELINE_GRAPH_H

#include <stdint.h>

typedef struct
{
    uint32_t op_code;
    void *params;
} img_pipeline_node_t;

typedef struct
{
    img_pipeline_node_t *nodes;
    uint32_t node_count;
} img_pipeline_graph_t;

#endif