// /* api/v1/img_core.h */
// #ifndef IMGENGINE_API_CORE_H
// #define IMGENGINE_API_CORE_H

// #include <stdint.h>
// #include "img_error.h"
// #include "img_types.h"
// #include "img_pipeline.h"

// #ifdef __cplusplus
// extern "C"
// {
// #endif

//     // Opaque Engine Handle
//     typedef struct img_engine_s *img_engine_t;

//     /**
//      * @brief Initialize engine with N workers
//      */
//     img_engine_t img_api_init(uint32_t workers);

//     /**
//      * @brief Shutdown engine and free resources
//      */
//     void img_api_shutdown(img_engine_t engine);

//     /**
//      * @brief Fast path processing (file → file)
//      */
//     img_result_t img_api_process_fast(
//         img_engine_t engine,
//         const char *input_path,
//         const char *output_path,
//         uint32_t width,
//         uint32_t height);

//     /**
//      * @brief Advanced pipeline execution (future SaaS API)
//      */
//     img_result_t img_api_process_pipeline(
//         img_engine_t engine,
//         const uint8_t *input,
//         size_t input_size,
//         img_pipeline_desc_t *pipeline,
//         uint8_t **output,
//         size_t *output_size);

// #ifdef __cplusplus
// }
// #endif

// #endif