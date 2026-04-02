/* memory/numa.c */

#define _GNU_SOURCE
#include "memory/numa.h"
#include <numa.h>
#include <stdlib.h>

int img_numa_get_node(void)
{
    if (numa_available() < 0)
        return 0;

    return numa_node_of_cpu(sched_getcpu());
}

void *img_numa_alloc_onnode(size_t size, int node)
{
    if (numa_available() < 0)
        return aligned_alloc(64, size);

    return numa_alloc_onnode(size, node);
}

void img_numa_free(void *ptr, size_t size)
{
    if (numa_available() < 0)
    {
        free(ptr);
        return;
    }

    numa_free(ptr, size);
}

// #include "memory/numa.h"

// #ifdef __linux__
// #include <numa.h>
// #endif

// void *img_numa_alloc_onnode(size_t size, int node)
// {
// #ifdef __linux__
//     return numa_alloc_onnode(size, node);
// #else
//     (void)node;
//     return NULL;
// #endif
// }