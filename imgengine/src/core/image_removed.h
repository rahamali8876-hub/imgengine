// #ifndef IMGENGINE_CORE_IMAGE_H
// #define IMGENGINE_CORE_IMAGE_H

// #include <stdint.h>

// typedef struct
// {
//     uint8_t *data;
//     uint32_t width;
//     uint32_t height;
//     uint32_t channels;
//     uint32_t stride;
// } img_buffer_t;

// static inline img_buffer_t img_buffer_create(
//     uint8_t *data,
//     uint32_t w,
//     uint32_t h,
//     uint32_t ch)
// {
//     img_buffer_t buf;
//     buf.data = data;
//     buf.width = w;
//     buf.height = h;
//     buf.channels = ch;

//     // 64-byte aligned stride
//     buf.stride = ((w * ch + 63) / 64) * 64;

//     return buf;
// }

// #endif



use >>>>>> api/v1/img_type.h