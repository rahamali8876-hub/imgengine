// // // imgengine/layout.h

#ifndef IMG_LAYOUT_H
#define IMG_LAYOUT_H

#include "image.h"

// forward declarations
struct img_ctx;
struct img_job;

typedef struct img_cell
{
    int x, y;
    int w, h;
} img_cell_t;

typedef struct img_layout_info
{
    img_cell_t *cells;
    int count;
} img_layout_info_t;

// API
void layout_store(struct img_ctx *ctx,
                  img_cell_t *cells,
                  int count);

img_layout_info_t *layout_get(struct img_ctx *ctx);

int layout_grid(img_t *canvas,
                const img_t *photo,
                const struct img_job *job,
                struct img_ctx *ctx);

#endif

// #ifndef IMG_LAYOUT_H
// #define IMG_LAYOUT_H

// #include "image.h"
// #include "api.h" // 🔥 REQUIRED for img_job_t

// // forward declaration
// struct img_ctx;

// typedef struct
// {
//     int x, y;
//     int w, h;
// } img_cell_t;

// typedef struct
// {
//     img_cell_t *cells;
//     int count;
// } img_layout_info_t;

// // API
// void layout_store(struct img_ctx *ctx,
//                   img_cell_t *cells,
//                   int count);

// img_layout_info_t *layout_get(struct img_ctx *ctx);

// int layout_grid(img_t *canvas,
//                 const img_t *photo,
//                 const img_job_t *job, // ✅ FIXED
//                 struct img_ctx *ctx);

// #endif
