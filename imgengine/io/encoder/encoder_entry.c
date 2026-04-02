// io/encoder/encoder_entry.c

#include "io/encoder/encoder_entry.h"
#include <turbojpeg.h>
#include <stdlib.h>

int img_encode_from_buffer(img_ctx_t *ctx,
                           img_buffer_t *buf,
                           uint8_t **out_data,
                           size_t *out_size)
{
    if (!ctx || !buf || !out_data || !out_size)
        return -1;

    // tjhandle tj = tjInitCompress();
    tjhandle tj = ctx->tj_encoder;

    if (!tj)
        return -1;

    unsigned char *jpegBuf = NULL; // TurboJPEG will allocate
    unsigned long jpegSize = 0;

    int width = buf->width;
    int height = buf->height;
    int pitch = buf->stride;

    int quality = 85; // 🔥 you can make this dynamic later

    // 🔥 ZERO-COPY INPUT (no memcpy here)
    if (tjCompress2(tj,
                    buf->data, // ← directly from SLAB
                    width,
                    pitch,
                    height,
                    TJPF_RGB,
                    &jpegBuf,
                    &jpegSize,
                    TJSAMP_444,
                    quality,
                    TJFLAG_FASTDCT) != 0)
    {
        // tjDestroy(tj);
        return -1;
    }

    // 🔥 TRANSFER OWNERSHIP (important)
    *out_data = jpegBuf;
    *out_size = jpegSize;

    tjDestroy(tj);
    return 0;
}

// #include <stdlib.h>
// #include <string.h>
// #include "core/context.h"
// #include "pipeline/jump_table.h"

// void img_execute_pipeline(img_ctx_t *ctx,
//                           img_buffer_t *buf,
//                           uint32_t *ops,
//                           void **params,
//                           uint32_t count)
// {
//     for (uint32_t i = 0; i < count; i++)
//     {
//         uint32_t op = ops[i];

//         // 🔥 ZERO BRANCH DISPATCH
//         g_jump_table_local->ops[op](ctx, buf, params[i]);
//     }
// }

// int img_encode_from_buffer(img_ctx_t *ctx,
//                            img_buffer_t *buf,
//                            uint8_t **out_data,
//                            size_t *out_size)
// {
//     // 🔥 allocate output buffer (could also slab if streaming)
//     size_t size = buf->width * buf->height * buf->channels;

//     uint8_t *out = malloc(size);
//     if (!out)
//         return -1;

//     // 🔥 copy (only place copy happens — unavoidable for encoding)
//     memcpy(out, buf->data, size);

//     *out_data = out;
//     *out_size = size;

//     return 0;
// }