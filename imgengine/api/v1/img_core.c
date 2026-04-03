// // api/v1/img_core.c

// #include "api/v1/img_core.h"
// #include "core/context_internal.h"
// #include "runtime/worker.h"
// #include <stdlib.h>

// img_engine_t img_api_init(uint32_t workers)
// {
//     img_engine_t engine = calloc(1, sizeof(*engine));

//     engine->worker_count = workers;
//     engine->workers = calloc(workers, sizeof(img_worker_t));

//     for (uint32_t i = 0; i < workers; i++)
//         img_worker_init(&engine->workers[i], i);

//     return engine;
// }

// void img_api_shutdown(img_engine_t engine)
// {
//     for (uint32_t i = 0; i < engine->worker_count; i++)
//     {
//         engine->workers[i].running = 0;
//     }

//     free(engine->workers);
//     free(engine);
// }