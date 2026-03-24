#include <stdio.h>
#include "imgengine/plugins/plugin_registry.h"

int run_plugins(img_ctx_t *ctx,
                img_t *canvas,
                const img_job_t *job)
{
    int count = 0;
    img_plugin_t **plugins = plugin_get_all(&count);

    for (int i = 0; i < count; i++)
    {
        img_plugin_t *p = plugins[i];

        // skip disabled plugins
        if (!p)
            continue;

        if (!p->process(ctx, canvas, job))
        {
            printf("❌ Plugin failed: %s\n", p->name);
            return 0;
        }
    }

    return 1;
}