// api/v1/img_api.h

#ifndef IMGENGINE_API_H
#define IMGENGINE_API_H

#include <stdint.h>
#include <stddef.h>

// Opaque handle (correct L7 pattern)
typedef struct img_engine_s img_engine_t;

// Lifecycle
img_engine_t *img_api_init(uint32_t workers);
void img_api_shutdown(img_engine_t *engine);

// Fast path processing
int img_api_process_fast(
    img_engine_t *engine,
    uint8_t *input,
    size_t input_size,
    uint8_t **output,
    size_t *output_size);

#endif

// #ifndef IMGENGINE_API_H
// #define IMGENGINE_API_H

// #include <stddef.h>
// #include <stdint.h>

// typedef struct img_engine_s img_engine_t;

// img_engine_t *img_api_init(void);

// int img_api_process_fast(
//     img_engine_t *engine,
//     uint8_t *input,
//     size_t input_size,
//     uint8_t **output,
//     size_t *output_size);

// void img_api_shutdown(img_engine_t *engine);

// #endif