// imgengine/layout.h
// #include "image.h"

// #ifndef IMG_LAYOUT_H
// #define IMG_LAYOUT_H

// int layout_grid(img_t *canvas,
//                 const img_t *photo,
//                 int cols, int rows,
//                 int gap, int padding,
//                 img_ctx_t *ctx);

// #endif

// imgengine/layout.h

#ifndef IMG_LAYOUT_H
#define IMG_LAYOUT_H

#include "image.h"
#include "api.h" // 🔥 REQUIRED for img_job_t
#include "context.h"

int layout_grid(img_t *canvas,
                const img_t *photo,
                const img_job_t *job,
                img_ctx_t *ctx);

#endif