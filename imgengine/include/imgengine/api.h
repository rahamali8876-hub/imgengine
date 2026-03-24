// include/imgengine/api.h

#ifndef IMG_API_H
#define IMG_API_H

// ❌ REMOVE context.h include (breaks cycle)

// forward declaration
struct img_ctx;

typedef enum
{
    IMG_FIT,
    IMG_FILL
} img_scale_mode_t;

typedef struct img_job
{
    float photo_w_cm;
    float photo_h_cm;
    int dpi;

    int bleed_px;
    int crop_mark_px;
    int crop_thickness;
    int crop_offset_px;

    int cols;
    int rows;
    int gap;

    int border_px;
    int padding;

    img_scale_mode_t mode;

    unsigned char bg_r;
    unsigned char bg_g;
    unsigned char bg_b;

} img_job_t;

// ✅ FIXED SIGNATURE
int imgengine_run(
    struct img_ctx *ctx,
    const char *input,
    const char *output,
    const img_job_t *job);

#endif
