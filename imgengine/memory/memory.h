// /* memory/memory.h */

// memory/memory.h (FIXED)
#ifndef IMGENGINE_MEMORY_H
#define IMGENGINE_MEMORY_H

#include "memory/slab.h"
#include "memory/arena.h"

#endif

// #ifndef IMGENGINE_MEMORY_H
// #define IMGENGINE_MEMORY_H

// #include <stddef.h>
// #include <stdint.h>
// #include <stdbool.h>

// // Performance Constants
// #define ALIGN_64 64
// #define HPAGE_SIZE (2 * 1024 * 1024) // 2MB HugePage

// // Opaque Slab Structures
// typedef struct img_slab_pool img_slab_pool_t;
// typedef struct img_arena img_arena_t;

// // --- HugePage Logic ---
// void *img_hugepage_alloc(size_t size);
// void img_hugepage_free(void *ptr, size_t size);

// // --- Slab Allocator (Hot Path) ---
// img_slab_pool_t *img_slab_init(size_t block_size, size_t num_blocks, int numa_node);
// void *img_slab_alloc(img_slab_pool_t *pool);
// void img_slab_free(img_slab_pool_t *pool, void *ptr);
// void img_slab_destroy(img_slab_pool_t *pool);

// // --- Arena Allocator (Thread-Local Metadata) ---
// img_arena_t *img_arena_create(size_t size);
// void *img_arena_alloc(img_arena_t *arena, size_t size);

// #endif
