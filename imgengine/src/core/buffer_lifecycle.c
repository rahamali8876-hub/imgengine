// ./src/core/buffer_lifecycle.c







// ./src/core/buffer_lifecycle.c

// ./src/core/buffer_lifecycle.c

// ./src/core/buffer_lifecycle.c

// ./src/core/buffer_lifecycle.c

// ./src/core/buffer_lifecycle.c

// src/core/buffer_lifecycle.c

#include "runtime/buffer_lifecycle.h"
#include "core/buffer.h"

void img_buffer_acquire(img_buffer_t *buf)
{
    if (!buf || !buf->data)
        return;

    img_buf_header_t *hdr = img_buf_hdr(buf);
    atomic_fetch_add_explicit(&hdr->ref, 1, memory_order_relaxed);
}

void img_buffer_release(img_buffer_t *buf)
{
    if (!buf || !buf->data)
        return;

    img_buf_header_t *hdr = img_buf_hdr(buf);

    if (atomic_fetch_sub_explicit(&hdr->ref, 1, memory_order_acq_rel) == 1)
    {
        // 🔥 RETURN TO SLAB (future hook)
        // For now: NO-OP (ownership handled by pipeline)
    }
}

// #include "runtime/buffer_lifecycle.h"
// // #include "api/v1/img_types.h"
// #include "core/buffer.h"
// #include <stdatomic.h>

// typedef struct
// {
//     _Atomic uint32_t ref;
// } ref_counter_t;

// void img_buffer_acquire(img_buffer_t *buf)
// {
//     if (!buf)
//         return;

//     ref_counter_t *rc = (ref_counter_t *)(buf->data - sizeof(ref_counter_t));
//     atomic_fetch_add(&rc->ref, 1);
// }

// void img_buffer_release(img_buffer_t *buf)
// {
//     if (!buf)
//         return;

//     ref_counter_t *rc = (ref_counter_t *)(buf->data - sizeof(ref_counter_t));

//     if (atomic_fetch_sub(&rc->ref, 1) == 1)
//     {
//         // 🔥 return to slab
//         // ctx not available → design decision: global pool fallback
//     }
// }