// plugins/plugin_inhernal.h

#ifndef IMGENGINE_PLUGINS_INTERNAL_H
#define IMGENGINE_PLUGINS_INTERNAL_H

#include "pipeline/jump_table.h" // 🔥 brings op_fn + batch_fn
#include "pipeline/batch.h"      // 🔥 brings img_batch_t
#include "api/v1/img_types.h"    // 🔥 brings img_buffer_t

// Unified OpCodes
#define OP_RESIZE 0x01
#define OP_CROP 0x02
#define OP_GRAYSCALE 0x03
#define OP_RESIZE_H 0x10
#define OP_RESIZE_V 0x11

// Plugin descriptor
typedef struct
{
    uint32_t op_code;
    const char *name;
    img_op_fn single_exec;
    img_batch_op_fn batch_exec;
} img_plugin_info_t;

// Init
void img_plugins_init_all(void);

// Plugin prototypes
void plugin_resize_single(img_ctx_t *, img_buffer_t *, void *);
void plugin_resize_batch(img_ctx_t *, img_batch_t *, void *);
void plugin_crop_single(img_ctx_t *, img_buffer_t *, void *);
void plugin_grayscale_single(img_ctx_t *, img_buffer_t *, void *);

#endif

// #ifndef IMGENGINE_PLUGINS_INTERNAL_H
// #define IMGENGINE_PLUGINS_INTERNAL_H

// #include "pipeline/pipeline.h"

// // Unified OpCodes (Engine-wide contract)
// #define OP_RESIZE 0x01
// #define OP_CROP 0x02
// #define OP_GRAYSCALE 0x03
// #define OP_RESIZE_H 0x10
// #define OP_RESIZE_V 0x11

// // Plugin descriptor (future dynamic loading support)
// typedef struct
// {
//     uint32_t op_code;
//     const char *name;
//     img_op_fn single_exec;
//     img_batch_op_fn batch_exec;
// } img_plugin_info_t;

// // Global registration entrypoint
// void img_plugins_init_all(void);

// // Plugin prototypes
// void plugin_resize_single(img_ctx_t *, img_buffer_t *, void *);
// void plugin_resize_batch(img_ctx_t *, img_batch_t *, void *);
// void plugin_crop_single(img_ctx_t *, img_buffer_t *, void *);
// void plugin_grayscale_single(img_ctx_t *, img_buffer_t *, void *);

// #endif