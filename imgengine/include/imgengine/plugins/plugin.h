// plugins/plugin.h

#ifndef IMG_PLUGIN_H
#define IMG_PLUGIN_H

#include "imgengine/image.h"
#include "imgengine/context.h"
#include "imgengine/api.h"

typedef struct img_plugin
{
    const char *name;

    // return 1 = success, 0 = fail
    int (*process)(img_ctx_t *ctx,
                   img_t *canvas,
                   const img_job_t *job);

} img_plugin_t;

#endif