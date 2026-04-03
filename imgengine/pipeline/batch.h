// pipeline/batch.h

#ifndef IMGENGINE_BATCH_H
#define IMGENGINE_BATCH_H

#include "api/v1/img_types.h"

typedef struct
{
    img_buffer_t *buffers;
    uint32_t count;
} img_batch_t;

#endif