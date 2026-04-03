/* src/cold/debug.h */
#ifndef IMGENGINE_COLD_DEBUG_H
#define IMGENGINE_COLD_DEBUG_H

#include "core/context_internal.h"

void img_debug_dump_slab_usage(const img_ctx_t *ctx);
void img_debug_print_cpu_info(void);

#endif