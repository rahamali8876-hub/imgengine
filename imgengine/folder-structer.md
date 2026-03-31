### 📁 FOLDER STRUCTURE

    imgengine/
    |   ├── include/                 # INTERNAL (The "How" - Detailed)
    │   ├── core/
    │   │   ├── context_internal.h
    │   │   ├── dispatcher_internal.h
    │   │   └── config_internal.h
    │   ├── arch/
    │   │   ├── cpu_caps.h       # SIMD feature bits
    │   │   └── arch_interface.h # Kernel signatures
    │   ├── memory/
    │   │   ├── slab_internal.h
    │   │   └── numa_node.h
    │   └── runtime/
    │       ├── worker_state.h
    │       └── spsc_barrier.h   # Atomic fence definitions
    ├── cmd/                        # EXECUTABLES (The "Main" entry points)
    │   ├── imgengine/              # THE PRIMARY CLI TOOL
    │   │   ├── main.c              # Argument parsing & worker bootstrap
    │   │   ├── args.c/h            # getopt_long logic (Kernel-grade CLI flags)
    │   │   └── shell.c/h           # Interactive mode (for debugging)
    │   └── bench/                  # PERFORMANCE SUITE
    │       ├── lat_bench.c         # P99 Latency measuring tool
    │       └── thr_bench.c         # 100K ops/sec stress tester
    ├── api/                        # PUBLIC, STABLE ABI (Versioned)
    │   └── v1/
    │       ├── img_core.h          # Opaque handles, init/shutdown
    │       ├── img_types.h         # Image metadata & buffer descriptors
    │       ├── img_pipeline.h      # User-facing pipeline builder
    │       └── img_error.h         # IMG_SUCCESS, IMG_ERR_NOMEM, etc.
    │
    ├── src/                        # CORE IMPLEMENTATION
    │   ├── core/
    │   │   ├── context.c/h         # Thread-local state & Slab management
    │   │   ├── dispatcher.c/h      # Runtime CPUID (AVX-512/NEON) logic
    │   │   ├── config.c/h          # SaaS tuning (Batch size, HugePages)
    │   │   └── init.c/h            # Global bootstrap & Registry load
    │   ├── hot/                    # PERFORMANCE-CRITICAL (No branching)
    │   │   ├── pipeline_exec.c/h   # The Jump-Table execution loop
    │   │   ├── pixel_ops.h         # Always-inline SIMD math helpers
    │   │   └── batch_engine.c/h    # 8-way interleaved SIMD orchestrator
    │   └── cold/                   # NON-CRITICAL (Error/Validation)
    │       ├── validation.c/h      # Malicious header & bounds checking
    │       ├── error.c/h           # String formatting & log-routing
    │       └── debug.c/h           # Slab dump & CPU telemetry
    │
    ├── pipeline/                   # EXECUTION GRAPH ENGINE
    │   ├── engine.c/h              # DAG orchestrator
    │   ├── registry.c/h            # Internal plugin registration
    │   ├── jump_table.c/h          # Global function pointer arrays
    │   └── scheduler.c/h           # Batch-to-worker task mapping
    │
    ├── memory/                     # KERNEL-GRADE ALLOCATORS
    │   ├── slab.c/h                # O(1) Fixed-size image blocks
    │   ├── arena.c/h               # Thread-local metadata allocator
    │   ├── numa.c/h                # NUMA-node affinity (Socket pinning)
    │   └── hugepage.c/h            # 2MB Page mapping (TLB optimization)
    │
    ├── runtime/                    # THREADING & QUEUES
    │   ├── worker.c/h              # Pinned worker loop
    │   ├── queue_spsc.c/h          # Lock-free atomic ring buffer
    │   ├── affinity.c/h            # pthread_setaffinity_np logic
    │   └── backpressure.c/h        # Load-shedding & queue-limit logic
    │
    │
    ├── plugins/                    # STATIC EXTENSIONS
    │   ├── plugin_resize.c/h       # Resize wrapper
    │   ├── plugin_crop.c/h         # Zero-copy crop logic
    │   └── plugin_grayscale.c/h    # Color conversion plugin
    │
    ├── observability/              # METRICS & TRACING
    │   ├── metrics.c/h             # Prometheus/StatsD counters
    │   └── profiler.c/h            # RDTSC-based latency tracking
    │
    ├── build/                      # BUILD SYSTEM
    │   ├── CMakeLists.txt          # -O3 -march=native -fno-plt flags
    │   └── toolchain.cmake         # Cross-compilation settings
    |
    ├── io/                         # HARDENED I/O LAYER (Hostile Input Handling)
    │   ├── decoder/                # RAW TO SLAB (Inbound)
    │   │   ├── jpeg/
    │   │   │   ├── jpeg_turbo.c/h  # libjpeg-turbo SIMD bridge
    │   │   │   └── jpeg_marker.c/h # Fast-scan EOI/SOI marker validation
    │   │   ├── png/
    │   │   │   ├── png_spng.c/h    # libspng (security-hardened) bridge
    │   │   │   └── png_chunk.c/h   # Critical chunk (IHDR/PLTE) validation
    │   │   ├── webp/
    │   │   │   └── webp_native.c/h # libwebp-demux integration
    │   │   └── stb_bridge.c/h      # Legacy fallback (fuzz-tested)
    │   ├── encoder/                # SLAB TO RAW (Outbound)
    │   │   ├── jpeg_encoder.c/h    # Quality-tuned Huffman destination manager
    │   │   └── png_encoder.c/h     # Z-lib strategy optimization
    │   └── vfs/                    # VIRTUAL FILE SYSTEM (Zero-Copy abstraction)
    │       ├── memory_stream.c/h   # RAM-to-Slab streaming logic
    │       ├── posix_io.c/h        # mmap() and pread() wrappers
    │       ├── s3_adapter.c/h      # Range-request logic for cloud objects
    │       └── io_uring.c/h        # Linux 5.10+ async I/O syscall optimization
    │
    ├── arch/                       # SILICON LAYER (Hardware-Specific Kernels)
    │   ├── x86_64/                 # INTEL / AMD (High-Throughput)
    │   │   ├── avx2/
    │   │   │   ├── resize_avx2.c   # 256-bit Bilinear/Bicubic kernels
    │   │   │   ├── color_avx2.c    # YUV/RGB/Gray conversion
    │   │   │   └── filters_avx2.c  # Convolution/Blur kernels
    │   │   ├── avx512/             # GOOGLE-GRADE SERVER OPTIMIZATION
    │   │   │   ├── resize_fma.c    # 512-bit Fused-Multiply-Add optimization
    │   │   │   ├── mask_logic.c    # Edge-case masking for non-aligned widths
    │   │   │   └── color_zmm.c     # Maximum IPC luminance kernels
    │   │   └── scalar/
    │   │       └── fallback.c      # Bit-exact reference implementation
    │   └── aarch64/                # ARM / GRAVITON / APPLE (Efficiency)
    │       ├── neon/
    │       │   ├── resize_neon.c   # 128-bit AdvSIMD kernels
    │       │   ├── interleave.c    # vld3/vst3 interleaved RGB optimization
    │       │   └── color_neon.c    # Fixed-point color math
    │       ├── sve/                # FUTURE-PROOF (Scalable Vector Extensions)
    │       │   └── vector_agnostic.c # Auto-sizing vector logic
    │       └── scalar/
    │           └── reference.c     # ARM-specific scalar logic
    │
    └── include/arch/               # ARCHITECTURE INTERNAL HEADERS
        ├── cpu_caps.h              # CPUID/Feature-bit bitmasks
        └── arch_interface.h        # Unified kernel signatures for Jump Table

    │
    └── Makefile                    # Principal-level master build script
