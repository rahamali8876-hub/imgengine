// src/core/init.c

#include "core/init.h"
#include "pipeline/hardware_registry.h"
#include "plugins/plugin_internal.h"
#include "observability/metrics.h"
#include "arch/cpu_caps.h"

void img_engine_bootstrap(void)
{
    // 1. Metrics
    img_metrics_init();

    // 2. Hardware binding (FIRST)
    cpu_arch_t arch = img_detect_cpu();
    img_registry_init_hardware(arch);

    // 3. Plugin overrides (SECOND)
    img_plugins_init_all();
}