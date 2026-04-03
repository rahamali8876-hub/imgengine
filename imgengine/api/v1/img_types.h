/* api/v1/img_types.h */

/* api/v1/img_types.h */
#ifndef IMGENGINE_API_TYPES_H
#define IMGENGINE_API_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint32_t width;
        uint32_t height;
        uint32_t channels; // 3 = RGB, 4 = RGBA
        uint32_t stride;
        uint8_t *data;
    } img_buffer_t;

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

#endif // IMGENGINE_API_TYPES_H

// #ifndef IMGENGINE_API_TYPES_H
// #define IMGENGINE_API_TYPES_H

// #include <stdint.h>
// #include <stddef.h>

// #ifdef __cplusplus
// extern "C"
// {
// #endif

//     typedef struct
//     {
//         uint32_t width;
//         uint32_t height;
//         uint32_t channels; // 3 = RGB, 4 = RGBA
//         uint32_t stride;
//         uint8_t *data;
//     } img_buffer_t;

//     typedef struct
//     {
//         uint32_t op_code;
//         void *params;
//     } img_op_desc_t;

// #define IMG_MAX_PIPELINE_OPS 16

//     typedef struct
//     {
//         img_op_desc_t ops[IMG_MAX_PIPELINE_OPS];
//         uint32_t count;
//     } img_pipeline_desc_t;

// #ifdef __cplusplus
// }
// #endif

// // #endif

// // #ifndef IMGENGINE_CORE_IMAGE_H
// // #define IMGENGINE_CORE_IMAGE_H

// // #include <stdint.h>

// // typedef struct
// // {
// //     uint8_t *data;
// //     uint32_t width;
// //     uint32_t height;
// //     uint32_t channels;
// //     uint32_t stride;
// // } img_buffer_t;

// // static inline img_buffer_t img_buffer_create(
// //     uint8_t *data,
// //     uint32_t w,
// //     uint32_t h,
// //     uint32_t ch)
// // {
// //     img_buffer_t buf;
// //     buf.data = data;
// //     buf.width = w;
// //     buf.height = h;
// //     buf.channels = ch;

// //     // 64-byte aligned stride
// //     buf.stride = ((w * ch + 63) / 64) * 64;

// //     return buf;
// // }

// // #endif