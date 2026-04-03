// // runtime/task.h
#ifndef IMGENGINE_RUNTIME_TASK_H
#define IMGENGINE_RUNTIME_TASK_H

#include <stdint.h>
#include <stddef.h>
#include "api/v1/img_types.h"

typedef struct
{
    // Input
    const uint8_t *input;
    size_t input_size;

    // Pipeline
    img_pipeline_desc_t *pipeline;

    // Output
    uint8_t *output;
    size_t output_size;

    // Result
    int status;

} img_task_t;

#endif

// #ifndef IMGENGINE_RUNTIME_TASK_H
// #define IMGENGINE_RUNTIME_TASK_H

// #include <stdint.h>
// #include <stddef.h>

// typedef struct
// {
//     // Input
//     uint8_t *input_data;
//     size_t input_size;

//     // Output
//     uint8_t *output_data;
//     size_t output_size;

//     uint32_t *ops;
//     void **params;
//     uint32_t op_count;

// } img_task_t;

// #endif