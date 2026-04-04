🔥 ImgEngine Runtime — Full Worker System (L7/L8 Design)
🧠 Core Philosophy

RFC: imgengine v1.0 (FINAL)

    Title: High-Performance, Kernel-Grade Image Processing Engine
    Author: Principal Engineer (L7)
    Status: Final Draft
    Date: 2026-03-30

You are building:

A NUMA-aware, lock-free, backpressure-controlled execution engine

Not just threads.

🧱 1. FINAL RUNTIME ARCHITECTURE
                ┌──────────────────────────┐
                │      API (img_api_*)     │
                └────────────┬─────────────┘
                             │
                             ▼
                ┌──────────────────────────┐
                │     Scheduler Layer      │
                │  (routing + load split)  │
                └────────────┬─────────────┘
                             │
        ┌────────────────────┼────────────────────┐
        ▼                    ▼                    ▼
 ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
 │ Worker 0    │     │ Worker 1    │     │ Worker N    │
 │ (NUMA 0)    │     │ (NUMA 1)    │     │ (NUMA X)    │
 │             │     │             │     │             │
 │ SPSC Queue  │     │ SPSC Queue  │     │ SPSC Queue  │
 │ Slab Pool   │     │ Slab Pool   │     │ Slab Pool   │
 │ Arena       │     │ Arena       │     │ Arena       │
 └─────┬───────┘     └─────┬───────┘     └─────┬───────┘
       ▼                   ▼                   ▼
   pipeline_exec       pipeline_exec       pipeline_exec



ImgEngine: High-Performance, Kernel-Grade Image Processing Engine

Author: Principal Engineer (L7)
Status: Final Draft
Date: 2026-03-30

1. Overview

ImgEngine is a high-performance, zero-copy, kernel-grade image processing engine designed for production workloads:

Input/Output: JPEG, PNG, PDF (future rasterization)
Transformations: Separable Resize, Crop, Grayscale, Bleed (experimental)
Execution: CPU-aware, multi-threaded, pipeline-oriented
Memory: Slab-based zero-copy allocation, ASAN/poison support
Observability: Metrics, tracing, profiling, async logging
Security: Bounds checks, input validation, sandboxing

It is SIMD-optimized, supports AVX2, AVX512, NEON, and has scalar fallbacks for maximum portability.

2. Architecture Layers
2.1 IO Layer (imgengine/io/)

Handles streams and decoders:

Memory streams: img_vfs_open_mem
File streams: img_vfs_mmap_file
S3 adapter: Header fetch, partial reads
Decoders: TurboJPEG, STB (zero-copy into slab)
Encoders: TurboJPEG (zero-copy)
Design Goal: Decouple file/network IO from core processing, optimize memory bandwidth.
2.2 Architecture Layer (imgengine/arch/)

Provides CPU-specific, SIMD-accelerated kernels:

CPU Detection: ARCH_SCALAR | ARCH_AVX2 | ARCH_AVX512 | ARCH_NEON
Unified Kernel Interface:
typedef void (*img_kernel_fn)(img_ctx_t *ctx, img_buffer_t *buf, void *params);
Optimized Kernels:
x86_64: AVX2 (resize_h_avx2, resize_v_avx2), AVX512 (resize_avx512), scalar fallback
ARM64: NEON (resize_h_neon, resize_v_neon), scalar fallback
Separable Resize: Horizontal + vertical passes
Grayscale: SIMD & scalar implementations
2.3 Plugins Layer (imgengine/plugins/)

Encapsulates reusable operations:

Plugin	Operation	Notes
plugin_resize	OP_RESIZE	Horizontal + Vertical passes, precomputed indices, slab-based temp buffers
plugin_crop	OP_CROP	Pointer shift, zero-copy
plugin_grayscale	OP_GRAYSCALE	SIMD/scalar implementations
plugin_bleed	OP_BLEED	Experimental
plugin_pdf	OP_PDF	PDF rasterization (future)

Jump Table Registration:

img_plugins_init_all(); // O(1) plugin dispatch

Design Principles:

single_exec → single image
batch_exec → multi-image pipelines
Zero-copy wherever possible
2.4 Pipeline Layer (imgengine/pipeline/)
Orchestrates sequential transformations
Jump table-based dispatch for O(1) operation selection
Supports batch execution for multi-image pipelines
Optimized for hot path performance: pre-allocated slabs, SIMD kernels, minimal branching

Example Hot Path Flow:

decode → resize_h → resize_v → grayscale → crop → encode
2.5 Memory Management
Slab Allocator: img_slab_alloc, img_slab_free
Context (img_ctx_t): Holds per-thread pools, decoder/encoder handles, CPU dispatch info
Zero-Copy Philosophy: Transformations operate directly on pre-allocated buffers
Memory Safety / ASAN:
IMG_POISON_MEMORY(addr, size);
IMG_UNPOISON_MEMORY(addr, size);
2.6 Security Layer (imgengine/security/)
Bounds Checking: img_bounds_check ensures pointer + size within buffer
Input Validation: img_security_validate_request prevents oversized or malicious images
Sandboxing: Seccomp filter to restrict process syscalls
Allowed: read, write, exit
Blocked: execve, socket, open, chmod
ASAN Integration: Optional memory poisoning for debug/fuzzing
2.7 Observability Layer (imgengine/observability/)

Provides runtime monitoring, tracing, and profiling:

Logging: Async SPSC queue (logger.c)
Metrics: Latency, request count, backpressure drops, slab usage (metrics.c)
Profiler: img_profiler_now for cycle-accurate timing (profiler.c)
Tracing: OpenTelemetry-compatible spans (tracing.c)

Metrics Export Example (Prometheus):

img_latency_avg 123
img_latency_max 456
img_requests_total 789
img_backpressure_drops 0

Tracing Example:

img_span_t span = img_trace_span_start("SIMD_RESIZE");
// ... do work ...
img_trace_span_end(span);
3. Execution Model
CPU Detection: img_detect_cpu() selects SIMD or scalar kernel
Plugin Dispatch: Jump table for O(1) execution
Task Runtime: Future per-thread workers, NUMA-aware slab pools
Batch Processing: Horizontal + vertical passes with precomputed index tables
Zero-Copy Hot Path: All operations operate directly on slabs, no intermediate allocations
4. Example Usage
img_ctx_t ctx;
img_buffer_t buf;

// 1. Load image
img_vfs_open_mem(&stream, jpeg_bytes, jpeg_size);
img_decode_to_buffer(&ctx, stream.data, stream.size, &buf);

// 2. Resize
resize_params_t params = { .target_w = 1024, .target_h = 768 };
plugin_resize_single(&ctx, &buf, &params);

// 3. Grayscale
plugin_grayscale_single(&ctx, &buf, NULL);

// 4. Crop
crop_params_t crop = { .x=10, .y=10, .w=512, .h=512 };
plugin_crop_single(&ctx, &buf, &crop);

// 5. Encode
uint8_t *out_data;
size_t out_size;
img_encode_from_buffer(&ctx, &buf, &out_data, &out_size);
5. Future L7 Enhancements
Full worker runtime (multi-threaded, lock-free, NUMA-aware)
Pipeline-as-a-Service: HTTP/gRPC endpoints
SIMD Optimizations: AVX512 full-width, ARM SVE
Advanced observability: Prometheus, Jaeger tracing, async exporters
Security hardening: ASAN, fuzzing, memory tagging
6. Key Principles
Zero-Copy Design → Maximum memory bandwidth
Separable Kernels → High throughput, SIMD-friendly
CPU-Agnostic → Runtime SIMD dispatch with scalar fallback
Pipeline + Plugins + Slabs → Composable, safe, production-ready
Observability & Security Built-In → Metrics, tracing, bounds-checking, sandboxing
