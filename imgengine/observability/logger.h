// observability/logger.h

#ifndef IMGENGINE_LOGGER_H
#define IMGENGINE_LOGGER_H

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} img_log_level_t;

void img_log_write(img_log_level_t level, const char *fmt, ...);

#endif