/* include/arch/cpu_caps.h */
#ifndef IMGENGINE_ARCH_CPU_CAPS_H
#define IMGENGINE_ARCH_CPU_CAPS_H

#include <stdint.h>

typedef enum
{
    ARCH_SCALAR = 0,
    ARCH_AVX2,
    ARCH_AVX512,
    ARCH_NEON
} cpu_arch_t;

/**
 * @brief Detect CPU capabilities (implemented in dispatcher.c)
 */
cpu_arch_t img_detect_cpu(void);

#endif