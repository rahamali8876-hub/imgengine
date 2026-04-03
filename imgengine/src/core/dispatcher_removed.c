// // src/core/dispatcher.c

// #include "core/dispatcher.h"

// cpu_arch_t img_detect_cpu(void)
// {
// #if defined(__AVX512F__)
//     return ARCH_AVX512;
// #elif defined(__AVX2__)
//     return ARCH_AVX2;
// #elif defined(__ARM_NEON)
//     return ARCH_NEON;
// #else
//     return ARCH_SCALAR;
// #endif
// }
