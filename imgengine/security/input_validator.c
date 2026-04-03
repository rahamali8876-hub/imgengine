// secrity/input_validator.c

#include "security/input_validator.h"

img_result_t img_security_validate_request(
    uint32_t w,
    uint32_t h,
    size_t file_size)
{
    if (w == 0 || h == 0 || w > 16384 || h > 16384)
        return IMG_ERR_SECURITY;

    uint64_t pixels = (uint64_t)w * h;

    if (pixels > file_size * 500)
        return IMG_ERR_SECURITY;

    return IMG_SUCCESS;
}