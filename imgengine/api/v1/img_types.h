/* api/v1/img_types.h */

#ifndef IMGENGINE_API_TYPES_H
#define IMGENGINE_API_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Full definition of img_buffer_t
    typedef struct
    {
        uint32_t width;
        uint32_t height;
        uint32_t channels; // 3 = RGB, 4 = RGBA
        uint32_t stride;
        uint8_t *data;
    } img_buffer_t;

    // Pipeline operation descriptor
    typedef struct
    {
        uint32_t op_code;
        void *params;
    } img_op_desc_t;

#define IMG_MAX_PIPELINE_OPS 16

    typedef struct
    {
        img_op_desc_t ops[IMG_MAX_PIPELINE_OPS];
        uint32_t count;
    } img_pipeline_desc_t;

#ifdef __cplusplus
}
#endif

#endif
