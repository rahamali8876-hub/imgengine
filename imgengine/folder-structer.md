### 📁 FOLDER STRUCTURE

    imgengine/
    ├── 📄 CMakeLists.txt          # Build configuration
    ├── 📂 include/imgengine/      # Public & Internal Headers
    │   ├── api.h                  # Primary entry points
    │   ├── context.h              # Engine state management
    │   ├── memory_pool.h          # Custom allocator logic
    │   ├── image.h                # Core image structs/metadata
    │   ├── layout.h               # Grid & spacing definitions
    │   └── common.h               # Macros & shared types
    │
    ├── 📂 src/                    # Implementation
    │   ├── 📂 core/               # The Engine Room
    │   │   ├── memory_pool.c      # Performance-first allocation
    │   │   ├── image.c            # Buffer handling
    │   │   ├── context.c          # Lifecycle management
    │   │   └── libt_avx2.c        # SIMD/Hardware acceleration
    │   ├── 📂 ops/                # Atomic Image Operations
    │   │   ├── resize.c           │
    │   │   ├── crop.c             │
    │   │   └── border.c           │
    │   ├── 📂 layout/             # Arrangement Logic
    │   │   ├── grid_layout.c      │
    │   │   └── crop_marks.c       │
    │   ├── 📂 io/                 # Input/Output
    │   │   ├── stb_impl.c         # STB third-party bridge
    │   │   └── image_io.c         # High-level Load/Save
    │   ├── 📂 pipeline/           # Orchestration
    │   │   └── plugin_runner.c    # Execution flow
    │   └── 📂 api/                # Public Interface Impl
    │       └── api.c              │
    │
    ├── 📂 plugins/                # Modular Extensibility
    │   ├── registers_all.c        # Plugin bootstrap
    │   ├── plugin_registry.c      # Dynamic lookup table
    │   ├── bleed_plugin.c         │
    │   ├── crop_plugin.c          │
    │   └── pdf_plugin.c           │
    │
    tests/
    ├── 📂 assets/                  # The "Ground Truth" Data
    │   ├── 🖼️ 1x1_white.png        # Absolute minimum bounds test
    │   ├── 🖼️ 8x8_checker.png      # Perfect for verifying offset/crop math
    │   ├── 🖼️ 127x127_noise.png    # Tests non-power-of-two (NPOT) SIMD cleanup
    │   └── 🖼️ transparent_rgba.png  # Verifies alpha channel preservation
    │
    ├── 📂 unit/                    # Stateless "Pure Function" Tests
    │   ├── 📄 test_mem_pool.c      # Verifies 32-byte alignment & fragmentation
    │   ├── 📄 test_simd_ops.c      # "Differential Testing" (AVX2 vs. Scalar C)
    │   ├── 📄 test_image_math.c    # Aspect ratio & coordinate transformation logic
    │   └── 📄 test_color_conv.c    # Grayscale/RGBA conversion accuracy
    │
    ├── 📂 integration/             # Component Interaction (The "Pipe")
    │   ├── 📄 test_pipeline.c      # Tests: Load -> Resize -> Border -> Save
    │   ├── 📄 test_plugin_reg.c    # Verifies plugins can register & be found by ID
    │   └── 📄 test_context_flow.c  # Ensures 'Context' state persists across ops
    │
    ├── 📂 e2e/                     # The "Black Box" (CLI level)
    │   ├── 📜 cli_suite.sh         # Shell script: runs ./imgengine with various flags
    │   ├── 📂 expected/            # "Golden" images to compare against CLI output
    │   └── 📄 test_file_formats.c  # Tests STB's ability to handle JPG, PNG, BMP, TGA
    │
    ├── 📄 test_helpers.h           # The "Test Framework" Macros
    └── 📄 test_main.c              # The test runner (runs all suites)

    │
    ├── 📂 third_party/            # External Dependencies
    │   └── 📂 stb/                # stb_image & stb_image_write
    │
    └── 📂 cli/                    # Tooling
        └── main.c                 # Command-line interface


### FINAL ARCHITECTURE

    imgengine/
    ├── 📄 CMakeLists.txt              # Modular build: generates .so/.a and separate binaries
    ├── 📄 .clang-format               # 6-year pro: Enforce style across the team
    ├── 📄 .valgrindrc                 # Suppressions for STB/third-party noise
    │
    ├── 📂 include/                    # Public API (Stripped of internal logic)
    │   └── 📂 imgengine/
    │       ├── api.h                  # Versioning macros & opaque handles (opaque pointers)
    │       ├── types.h                # POD types (img_rgba_t, img_rect_t)
    │       └── error_codes.h          # IMG_SUCCESS, IMG_ERR_ALIGNMENT, etc.
    │
    ├── 📂 src/                        
    │   ├── 📂 core/                   # The "Library" Logic
    │   │   ├── 📄 internal.h          # Internal-only macros & struct definitions
    │   │   ├── 📄 context.c           # Thread-local storage/Global state
    │   │   ├── 📄 allocator.c         # SIMD-aware aligned_alloc/posix_memalign wrappers
    │   │   └── 📄 dispatcher.c        # CPUID check: routes calls to AVX2, NEON, or C-Scalar
    │   │
    │   ├── 📂 arch/                   # Platform-Specific Optimizations
    │   │   ├── 📄 generic_scalar.c    # Fallback implementation (Readable C)
    │   │   ├── 📄 x86_avx2.c          # __m256 intrinsics (Width % 32 cleanup)
    │   │   └── 📄 arm_neon.c          # float32x4_t intrinsics
    │   │
    │   ├── 📂 io/                     # I/O Boundary
    │   │   ├── 📄 stb_wrapper.c       # Implements the 'stb' bridge
    │   │   └── 📄 format_detect.c     # Magic-byte header validation
    │   │
    │   └── 📂 pipeline/               
    │       └── 📄 graph_executor.c    # If you're chaining plugins (The "Engine" part)
    │
    ├── 📂 plugins/                    # Dynamic/Static Extensions
    │   ├── 📄 plugin_interface.h      # Shared VTable/Function pointers
    │   └── 📄 plugin_manager.c        # dlopen/dlsym logic for runtime loading
    │
    ├── 📂 third_party/                # Vendored dependencies
    │   └── 📂 stb/
    │       ├── 📄 stb_image.h         # Version-locked
    │       └── 📄 stb_image_write.h
    │
    ├── 📂 tests/                      # The "Guardian" Suite
    │   ├── 📄 test_helpers.h          # Bit-exactness macros & Benchmarking timers
    │   ├── 📂 functional/             # "Does the feature work?"
    │   │   ├── 📄 test_resize_logic.c
    │   │   └── 📄 test_memory_leaks.c # Targeted Valgrind scenarios
    │   ├── 📂 regression/             # "Does the SIMD match Scalar?"
    │   │   └── 📄 test_simd_parity.c  # Fuzzing random buffers to find 1-bit drifts
    │   └── 📂 data/                   # Binary assets
    │       ├── 🖼️ corpus/             # Wide variety of corrupted/valid files
    │       └── 🖼️ reference/          # "Golden" output images for pixel-diffing
    │
    ├── 📂 cli/                        # The "Consumer"
    │   ├── 📄 args.h/c                # Robust arg parsing (argp or getopt_long)
    │   └── 📄 main.c                  # Thin wrapper around the API
    │
    └── 📂 scripts/                    # DevEx Automation
        ├── 📜 run_benchmarks.sh       # Cycles-per-pixel (CPP) reporting
        └── 📜 generate_coverage.sh    # LCOV/GCOV for the CI pipeline



### 🔥 3. IMPORTANT ORDER (CRITICAL)

    👉 Plugin execution order must be:

    1. layout_grid
    2. bleed plugin   ✅ FIRST
    3. crop marks     ✅ AFTER