// imgengine/plugins/plugin_register.h 

#ifndef IMG_PLUGIN_REGISTRY_H
#define IMG_PLUGIN_REGISTRY_H

#include "plugin.h"

void plugin_register(img_plugin_t *plugin);
void register_all_plugins();
img_plugin_t **plugin_get_all(int *count);

#endif