// secrity/bounds_check.h


#ifndef IMGENGINE_BOUNDS_CHECK_H
#define IMGENGINE_BOUNDS_CHECK_H

#include <stdint.h>
#include <stdbool.h>

static inline bool img_bounds_check(
    const void *ptr,
    size_t size,
    const void *base,
    size_t limit)
{
    uintptr_t p = (uintptr_t)ptr;
    uintptr_t b = (uintptr_t)base;

    return (p >= b) && (p + size <= b + limit);
}

#endif