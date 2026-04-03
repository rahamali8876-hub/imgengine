/* pipeline/pipeline.h */

#ifndef IMGENGINE_PIPELINE_H
#define IMGENGINE_PIPELINE_H

#include <stdint.h>
#include "api/v1/img_types.h"

// ================= PIPELINE =================

typedef struct
{
    uint32_t ops[IMG_MAX_PIPELINE_OPS];
    void *params[IMG_MAX_PIPELINE_OPS];
    uint32_t op_count;
} img_pipeline_t;

#endif

// #ifndef IMGENGINE_PIPELINE_H
// #define IMGENGINE_PIPELINE_H

// #include "api/v1/img_types.h" // reuse constant

// typedef struct
// {
//     uint32_t ops[IMG_MAX_PIPELINE_OPS];
//     void *params[IMG_MAX_PIPELINE_OPS];
//     uint32_t op_count;
// } img_pipeline_t;

// #endif

// #ifndef IMGENGINE_PIPELINE_H
// #define IMGENGINE_PIPELINE_H

// #include <stdint.h>

// #define IMG_MAX_PIPELINE_OPS 32

// typedef struct
// {
//     uint32_t ops[IMG_MAX_PIPELINE_OPS];
//     void *params[IMG_MAX_PIPELINE_OPS];
//     uint32_t op_count;
// } img_pipeline_t;

// #endif

// #ifndef IMGENGINE_PIPELINE_H
// #define IMGENGINE_PIPELINE_H

// #include <stdint.h>
// #include "core/context.h"
// #include "core/image.h"
// #include "hot/batch.h"

// #define MAX_OPCODES 256

// // --- Function Pointer Types ---
// typedef void (*img_op_fn)(img_ctx_t *ctx, img_buffer_t *buf, void *params);
// typedef void (*img_batch_op_fn)(img_ctx_t *ctx, img_batch_t *batch, void *params);

// // --- Global Jump Tables ---
// extern img_op_fn g_jump_table[MAX_OPCODES];
// extern img_batch_op_fn g_batch_jump_table[MAX_OPCODES];

// // --- DAG Node ---
// typedef struct
// {
//     uint32_t op_code;
//     void *params;
// } img_dag_node_t;

// // --- Pipeline Graph ---
// typedef struct
// {
//     img_dag_node_t nodes[16];
//     uint32_t node_count;
// } img_pipeline_graph_t;

// #endif