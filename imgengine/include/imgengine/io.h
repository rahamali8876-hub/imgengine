#ifndef IMG_IO_H
#define IMG_IO_H

#include "image.h"

int img_load(const char *path, img_t *img);
int img_save_png(const char *path, const img_t *img);
int img_save_pdf(const char *filename, const img_t *img);

#endif