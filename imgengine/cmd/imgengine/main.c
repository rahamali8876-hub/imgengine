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

// #include "api/v1/img_core.h"
// #include "args.h"
// #include <stdio.h>
// #include <signal.h>

// static volatile bool keep_running = true;
// void handle_sigint(int sig)
// {
//     (void)sig;
//     keep_running = false;
// }

// int main(int argc, char **argv)
// {
//     img_cli_options_t opts = {0};
//     if (img_parse_args(argc, argv, &opts) != 0)
//     {
//         img_print_usage(argv[0]);
//         return 1;
//     }

//     // 1. KERNEL-GRADE BOOTSTRAP
//     // Initializes Slabs, HugePages, and Pins Workers to Cores
//     img_engine_t engine = img_api_init(opts.threads > 0 ? opts.threads : 1);
//     if (!engine)
//     {
//         fprintf(stderr, "[FATAL] Failed to initialize imgengine core\n");
//         return 1;
//     }

//     // 2. SIGNAL HANDLING (Clean exit on Ctrl+C)
//     signal(SIGINT, handle_sigint);

//     if (opts.verbose)
//         printf("[INFO] Processing %s -> %s\n", opts.input_path, opts.output_path);

//     // 3. EXECUTE PIPELINE (Direct-to-Slab)
//     img_result_t res = img_api_process_fast(engine, opts.input_path, opts.output_path,
//                                             opts.width, opts.height);

//     if (res != IMG_SUCCESS)
//     {
//         fprintf(stderr, "[ERROR] Pipeline failed with code: %d\n", res);
//     }

//     // 4. GRACEFUL SHUTDOWN
//     img_api_shutdown(engine);
//     return (res == IMG_SUCCESS) ? 0 : 1;
// }
