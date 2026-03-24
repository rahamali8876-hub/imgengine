// src/plugins/register_all.c 

#include "imgengine/plugins/plugin_registry.h"

img_plugin_t *get_bleed_plugin();
img_plugin_t *get_crop_plugin();

void register_all_plugins()
{
    plugin_register(get_bleed_plugin());
    plugin_register(get_crop_plugin());
    // register_plugin(get_bleed_plugin());
    // register_plugin(get_crop_plugin());
}