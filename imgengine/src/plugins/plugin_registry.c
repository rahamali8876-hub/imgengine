#include "imgengine/plugins/plugin.h"

#define MAX_PLUGINS 32

static img_plugin_t *g_plugins[MAX_PLUGINS];
static int g_plugin_count = 0;

void plugin_register(img_plugin_t *plugin)
{
    if (g_plugin_count < MAX_PLUGINS)
        g_plugins[g_plugin_count++] = plugin;
}

img_plugin_t **plugin_get_all(int *count)
{
    *count = g_plugin_count;
    return g_plugins;
}