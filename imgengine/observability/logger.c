// observability/logger.c 

#include "observability/logger.h"
#include "runtime/queue_spsc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static img_queue_t *g_log_queue;

void img_log_write(img_log_level_t level, const char *fmt, ...)
{
    if (level < LOG_INFO)
        return;

    char *msg = malloc(512);
    if (!msg)
        return;

    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, 512, fmt, args);
    va_end(args);

    if (!img_queue_push(g_log_queue, msg))
        free(msg); // drop log if full
}