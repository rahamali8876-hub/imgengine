// include/hot/pipelined_compiled.h

#ifndef IMGENGINE_PIPELINE_COMPILED_H
#define IMGENGINE_PIPELINE_COMPILED_H

#include <stdint.h>
#include "core/arch_interface.h"

// forward declaration (L8 rule)
typedef struct img_pipeline_desc img_pipeline_desc_t;

#define IMG_MAX_PIPELINE_OPS 32

typedef struct
{
    uint32_t count;

    // 🔥 DIRECT KERNEL POINTERS (RESOLVED ONCE)
    img_kernel_fn ops[IMG_MAX_PIPELINE_OPS];

    void *params[IMG_MAX_PIPELINE_OPS];

} img_pipeline_compiled_t;

// compile step (cold path)
int img_pipeline_compile(
    const img_pipeline_desc_t *in,
    img_pipeline_compiled_t *out);

#endif
