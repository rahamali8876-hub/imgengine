// pipeline/hardware_registry.c

#include "pipeline/jump_table.h"
#include "arch/arch_interface.h"
#include "plugins/plugin_internal.h"

void img_registry_init_hardware(cpu_arch_t arch)
{
    switch (arch)
    {
    case ARCH_AVX512:
    case ARCH_AVX2:

        // 🔥 Separable passes (SIMD)
        img_pipeline_register_op(OP_RESIZE_H, img_arch_resize_h_avx2, NULL);
        img_pipeline_register_op(OP_RESIZE_V, img_arch_resize_v_avx2, NULL);

        break;

    case ARCH_NEON:

        img_pipeline_register_op(OP_RESIZE_H, img_arch_resize_h_neon, NULL);
        img_pipeline_register_op(OP_RESIZE_V, img_arch_resize_v_neon, NULL);

        break;

    default:

        img_pipeline_register_op(OP_RESIZE_H, img_arch_resize_h_scalar, NULL);
        img_pipeline_register_op(OP_RESIZE_V, img_arch_resize_v_scalar, NULL);

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
//         img_pipeline_register_op(OP_RESIZE, img_arch_resize_avx512, NULL);
//         break;

//     case ARCH_AVX2:
//         img_pipeline_register_op(OP_RESIZE, img_arch_resize_avx2, NULL);
//         break;

//     case ARCH_NEON:
//         img_pipeline_register_op(OP_RESIZE, img_arch_resize_neon, NULL);
//         break;

//     default:
//         img_pipeline_register_op(OP_RESIZE, img_arch_resize_scalar, NULL);
//         break;
//     }
// }

// #include "pipeline/hardware_registry.h"
// #include "pipeline/jump_table.h"
// #include "arch/arch_kernels.h"
// #include "plugins/plugin_internal.h"

// void img_registry_init_hardware(cpu_arch_t arch)
// {
//     switch (arch)
//     {
//     case ARCH_AVX512:
//         img_pipeline_register_op(OP_RESIZE, img_arch_avx512_resize, img_arch_batch_resize);
//         img_pipeline_register_op(OP_GRAYSCALE, img_arch_avx2_grayscale, NULL);
//         break;

//     case ARCH_AVX2:
//         img_pipeline_register_op(OP_RESIZE, img_arch_avx2_resize, img_arch_batch_resize);
//         img_pipeline_register_op(OP_GRAYSCALE, img_arch_avx2_grayscale, NULL);
//         break;

//     case ARCH_NEON:
//         img_pipeline_register_op(OP_RESIZE, img_arch_neon_resize, NULL);
//         break;

//     default:
//         img_pipeline_register_op(OP_RESIZE, img_arch_scalar_resize, NULL);
//         img_pipeline_register_op(OP_GRAYSCALE, img_arch_scalar_grayscale, NULL);
//         break;
//     }
// }

// #include "pipeline/jump_table.h"
// #include "arch/arch_kernels.h"
// #include "arch/cpu_caps.h"
// #include "plugins/plugin_internal.h"

// /**
//  * @brief Bind opcodes directly to hardware kernels (ZERO BRANCH HOT PATH)
//  */
// void img_registry_init_hardware(cpu_arch_t arch)
// {
//     switch (arch)
//     {
//     case ARCH_AVX512:
//         img_pipeline_register_op(OP_RESIZE, img_arch_avx512_resize, NULL);
//         break;

//     case ARCH_AVX2:
//         img_pipeline_register_op(OP_RESIZE, img_arch_avx2_resize, NULL);
//         break;

//     case ARCH_NEON:
//         img_pipeline_register_op(OP_RESIZE, img_arch_neon_resize, NULL);
//         break;

//     default:
//         img_pipeline_register_op(OP_RESIZE, img_arch_scalar_resize, NULL);
//         break;
//     }
// }