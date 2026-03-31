### ?? Suggested Build Order (imgengine Core)

    ?? Phase 0 — Foundation (Build System + Skeleton)
    Goal: Make the project compile cleanly
    Build:
    imgengine/
    +-- CMakeLists.txt
    +-- api/
    +-- src/
    +-- tests/
    Tasks:
    Setup CMake with:
    -O3
    -march=native
    Create empty .c/.h files
    Compile a dummy binary

    ?? Output:
    ? You can run make successfully

## ?? Phase 1 — Memory System (MOST IMPORTANT)

    Step 1: Slab Allocator
    File:
    memory/slab.c
    memory/slab.h
    Build:
    fixed-size block allocator
    pre-allocate large chunk
    free list
    Validate:
    allocate/free loop
    no malloc in loop
    Step 2: Thread-Local Arena
    Add:
    per-thread allocator
    no locks

    ?? Output:
    ? Deterministic allocation
    ? No fragmentation

## ? Phase 2 — Runtime Core

    Step 3: Context System
    core/context.c
    global + thread-local state
    allocator integration
    Step 4: Lock-Free Queue (SPSC)
    runtime/queue_spsc.c
    ring buffer
    no mutex
    Validate:
    producer ? consumer test

## ?? Phase 3 — Pipeline Engine

    Step 5: Plugin Interface
    pipeline/plugin.h

    Define:

    typedef void (*img_op_fn)(img_ctx_t*, img_buffer_t*);

    Step 6: Jump Table
    pipeline/jump_table.c
    array of function pointers
    static registration
    Step 7: Pipeline Executor
    pipeline/engine.c
    loop through ops
    call via jump table

    ?? Output:
    ? working pipeline (no SIMD yet)

## ??? Phase 4 — Image Core

    Step 8: Image Buffer
    core/image.c
    raw pixel buffer
    stride handling
    alignment (64-byte)
    Step 9: Basic Operation (Scalar)
    arch/scalar/resize.c
    simple resize or grayscale

    ?? Output:
    ? correct image processing

## ?? Phase 5 — SIMD Acceleration

    Step 10: AVX2 Implementation
    arch/x86_64/avx2/resize_avx2.c
    vectorized loop
    process multiple pixels
    Step 11: Dispatcher
    core/dispatcher.c
    detect CPU features
    select AVX2 or scalar

    ?? Output:
    ? automatic optimization

## ?? Phase 6 — Plugin Integration

    Step 12: Register Plugins
    plugins/plugin_resize.c
    bind resize to jump table
    Step 13: Build Full Pipeline
    resize ? crop ? output

    ?? Output:
    ? real pipeline working end-to-end

## ?? Phase 7 — I/O Layer

    Step 14: Image Decoder
    io/decoder_stb.c
    load JPEG/PNG
    Step 15: Output Writer
    io/output.c

    ?? Output:
    ? input.jpg ? processed ? output.jpg

## ?? Phase 8 — Testing & Benchmarking

    Step 16: Unit Tests
    allocator
    queue
    pipeline
    Step 17: Performance Benchmark
    scripts/bench_perf.c

    Measure:

    latency (P50/P99)
    throughput

## ?? Phase 9 — Optimization (Kernel-Level)

    Step 18: Cache Optimization
    align buffers
    prefetch memory
    Step 19: Batch Processing
    process N images at once
    Step 20: NUMA Awareness (optional advanced)
    per-core memory pools
    ?? Final Target
    input.jpg
    ?
    decode
    ?
    pipeline (jump table)
    ?
    SIMD resize (AVX2)
   ?
    output.jpg
    ?? Golden Rules During Build
    ? Don’t jump to SIMD early
    ? Don’t build too many features
    ? Don’t ignore testing
    ? Build small ? verify ? optimize
    ? Measure everything
    ? Keep hot path clean

## ?? Recommended Branch Strategy (Kernel-grade)

	main
  +-- feat/core-allocator
  +-- feat/runtime-queue
  +-- feat/pipeline-engine
  +-- feat/simd-avx2
  +-- feat/io-layer
  +-- feat/api-layer
