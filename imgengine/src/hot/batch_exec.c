// src/hot/batch_exec.c

#include "hot/batch_exec.h"
#include "pipeline/jump_table.h"
#include "pipeline/pipeline_types.h"
#include "pipeline/pipeline.h"

// 🔥 unroll factor (tunable)
#define UNROLL 2

void img_batch_execute(
    img_ctx_t *__restrict ctx,
    img_batch_t *__restrict batch,
    const img_pipeline_runtime_t *__restrict pipe)
{
    const uint32_t op_count = pipe->count;
    const img_op_desc_t *__restrict ops = pipe->ops;

    for (uint32_t op = 0; op < op_count; op++)
    {
        uint32_t opcode = ops[op].op_code;

        img_batch_op_fn fn = g_batch_jump_table[opcode];

        if (!fn)
            continue;

        // 🔥 batch-level execution
        fn(ctx, batch, ops[op].params);
    }
}
