// /* memory/arena.c */

#include "memory/arena.h"
#include <stdlib.h>
#include <string.h>

img_arena_t *img_arena_create(size_t size)
{
    img_arena_t *arena = malloc(sizeof(img_arena_t));
    if (!arena)
        return NULL;

    arena->base = aligned_alloc(64, size);
    if (!arena->base)
    {
        free(arena);
        return NULL;
    }

    arena->size = size;
    arena->offset = 0;

    return arena;
}

void *img_arena_alloc(img_arena_t *arena, size_t size)
{
    if (!arena || arena->offset + size > arena->size)
        return NULL;

    void *ptr = arena->base + arena->offset;
    arena->offset += (size + 63) & ~63; // align 64

    return ptr;
}

void img_arena_reset(img_arena_t *arena)
{
    if (arena)
        arena->offset = 0;
}

void img_arena_destroy(img_arena_t *arena)
{
    if (!arena)
        return;

    free(arena->base);
    free(arena);
}

// #include "memory/arena.h"

// void img_arena_init(img_arena_t* arena, void* memory, size_t size)
// {
//     arena->base = (uint8_t*)memory;
//     arena->size = size;
//     arena->offset = 0;
// }

// void* img_arena_alloc(img_arena_t* arena, size_t size)
// {
//     if (arena->offset + size > arena->size)
//         return NULL;

//     void* ptr = arena->base + arena->offset;
//     arena->offset += size;

//     return ptr;
// }

// void img_arena_reset(img_arena_t* arena)
// {
//     arena->offset = 0;
// }
