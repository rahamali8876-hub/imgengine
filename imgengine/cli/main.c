/* src/cli/main.c */
#include "api/v1/img_core.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: imgengine <in.jpg> <out.jpg> <width> <height>\n");
        return 1;
    }

    // 1. Bootstrap the Engine (Slabs, Pinning, SIMD Registry)
    img_engine_t engine = img_api_init(1); // 1 worker for CLI

    // 2. High-Speed Execution
    bool success = img_api_process_fast(engine, argv[1], argv[2],
                                        atoi(argv[3]), atoi(argv[4]));

    // 3. Clean Shutdown
    img_api_shutdown(engine);

    return success ? 0 : 1;
}
