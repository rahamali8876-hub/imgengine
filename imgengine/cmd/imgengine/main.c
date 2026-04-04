/* cmd/imgengine/main.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "api/v1/img_api.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: %s input.jpg output.jpg\n", argv[0]);
        return 1;
    }

    // =====================================
    // READ FILE
    // =====================================
    FILE *f = fopen(argv[1], "rb");
    if (!f)
        return 1;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *input = malloc(size);
    fread(input, 1, size, f);
    fclose(f);

    // =====================================
    // ENGINE
    // =====================================
    img_engine_t *engine = img_api_init();

    uint8_t *output = NULL;
    size_t out_size = 0;

    if (img_api_process_fast(
            engine,
            input,
            size,
            &output,
            &out_size) != 0)
    {
        printf("Processing failed\n");
        return 1;
    }

    // =====================================
    // WRITE FILE
    // =====================================
    FILE *out = fopen(argv[2], "wb");
    fwrite(output, 1, out_size, out);
    fclose(out);

    // =====================================
    // CLEANUP
    // =====================================
    free(input);
    free(output);

    img_api_shutdown(engine);

    printf("Done.\n");
    return 0;
}
