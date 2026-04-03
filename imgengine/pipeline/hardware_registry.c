// pipeline/hardware_registry.c

#include "pipeline/jump_table.h"
#include "arch/arch_interface.h"
#include "arch/cpu_caps.h"

// 🔥 Bind best kernels into jump table
void img_hw_register_kernels(cpu_arch_t arch)
{
    switch (arch)
    {
    case ARCH_AVX512:

        g_jump_table.ops[OP_RESIZE_H] = img_arch_resize_h_avx2; // fallback if AVX512 missing
        g_jump_table.ops[OP_RESIZE_V] = img_arch_resize_v_avx2;

        g_jump_table.ops[OP_GRAYSCALE] = img_arch_grayscale_avx2;

        break;

    case ARCH_AVX2:

        g_jump_table.ops[OP_RESIZE_H] = img_arch_resize_h_avx2;
        g_jump_table.ops[OP_RESIZE_V] = img_arch_resize_v_avx2;

        g_jump_table.ops[OP_GRAYSCALE] = img_arch_grayscale_avx2;

        break;

    case ARCH_NEON:

        g_jump_table.ops[OP_RESIZE_H] = img_arch_resize_h_neon;
        g_jump_table.ops[OP_RESIZE_V] = img_arch_resize_v_neon;

        break;

    default: // SCALAR

        g_jump_table.ops[OP_RESIZE_H] = img_arch_resize_h_scalar;
        g_jump_table.ops[OP_RESIZE_V] = img_arch_resize_v_scalar;

        g_jump_table.ops[OP_GRAYSCALE] = img_arch_grayscale_scalar;

        break;
    }
}

// #include "pipeline/jump_table.h"
// #include "arch/arch_interface.h"
// #include "plugins/plugin_internal.h"

// void img_registry_init_hardware(cpu_arch_t arch)
// {
//     switch (arch)
//     {
//     case ARCH_AVX512:
//     case ARCH_AVX2:

//         // 🔥 Separable passes (SIMD)
//         img_pipeline_register_op(OP_RESIZE_H, img_arch_resize_h_avx2, NULL);
//         img_pipeline_register_op(OP_RESIZE_V, img_arch_resize_v_avx2, NULL);

//         break;

//     case ARCH_NEON:

//         img_pipeline_register_op(OP_RESIZE_H, img_arch_resize_h_neon, NULL);
//         img_pipeline_register_op(OP_RESIZE_V, img_arch_resize_v_neon, NULL);

//         break;

//     default:

//         img_pipeline_register_op(OP_RESIZE_H, img_arch_resize_h_scalar, NULL);
//         img_pipeline_register_op(OP_RESIZE_V, img_arch_resize_v_scalar, NULL);

//         break;
//     }
// }
