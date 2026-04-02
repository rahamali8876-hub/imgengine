/* memory/slab.c */
#include "memory/slab.h"
#include <stdlib.h>
#include <string.h>
#include <stdalign.h>

typedef struct slab_block
{
    struct slab_block *next;
} slab_block_t;

struct img_slab_pool
{
    slab_block_t *free_list;
    void *memory;
    size_t total_size;
    size_t block_size;
    size_t block_count;
    int numa_node; // 🔥 NEW
};

img_slab_pool_t *img_slab_create(size_t total_size, size_t block_size)
{
    img_slab_pool_t *pool = malloc(sizeof(img_slab_pool_t));
    if (!pool)
        return NULL;

    // Align block size to 64 bytes
    block_size = (block_size + 63) & ~63;

    // pool->memory = aligned_alloc(64, total_size);
    int node = img_numa_get_node();
    pool->memory = img_numa_alloc_onnode(total_size, node);
    pool->numa_node = node;

    pool->total_size = total_size;
    pool->block_size = block_size;
    pool->block_count = total_size / block_size;
    pool->free_list = NULL;

    // Initialize free list
    uint8_t *ptr = (uint8_t *)pool->memory;
    for (size_t i = 0; i < pool->block_count; i++)
    {
        slab_block_t *block = (slab_block_t *)(ptr + i * block_size);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    return pool;
}

uint8_t *img_slab_alloc(img_slab_pool_t *pool)
{
    if (!pool || !pool->free_list)
        return NULL;

    slab_block_t *block = pool->free_list;
    pool->free_list = block->next;

    return (uint8_t *)block;
}

void img_slab_free(img_slab_pool_t *pool, void *ptr)
{
    if (!pool || !ptr)
        return;

    slab_block_t *block = (slab_block_t *)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
}

void img_slab_destroy(img_slab_pool_t *pool)
{
    if (!pool)
        return;

    img_numa_free(pool->memory, pool->total_size);
    free(pool);
}
