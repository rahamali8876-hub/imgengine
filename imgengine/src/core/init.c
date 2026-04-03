// src/core/init.c
#include "core/context_internal.h"
#include "arch/cpu_caps.h"
#include "pipeline/jump_table.h"

// forward
void img_plugins_init_all(void);
void img_hw_register_kernels(cpu_caps_t caps);

void img_engine_init(img_engine_t *engine)
{
    if (!engine)
        return;

    // ================= 1. DETECT CPU =================
    engine->caps = img_cpu_detect_caps();

    // ================= 2. INIT JUMP TABLE =================
    img_jump_table_init(engine->caps);

    // ================= 3. REGISTER PLUGINS =================
    img_plugins_init_all();

    // ================= 4. OPTIONAL SIMD OVERRIDE =================
    img_hw_register_kernels(engine->caps);
}

// #include "arch/cpu_caps.h"
// #include "pipeline/jump_table.h"

// // forward
// void img_plugins_init_all(void);
// void img_hw_register_kernels(cpu_arch_t arch);

// void img_engine_init(img_engine_t *engine)
// {
//     if (!engine)
//         return;

//     // 🔥 1. Detect CPU
//     cpu_arch_t arch = img_detect_cpu();
//     // engine->caps = arch;
//     engine->caps = img_cpu_detect_caps();
//     // 🔥 THIS IS THE MAGIC LINE
//     img_jump_table_init(engine->caps);

//     // 🔥 2. Register base plugins
//     img_plugins_init_all();

//     // 🔥 3. Override with best SIMD kernels
//     img_hw_register_kernels(arch);
// }
