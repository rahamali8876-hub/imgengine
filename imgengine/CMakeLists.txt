# # imgengine/CMakeLists.txt
# cmake_minimum_required(VERSION 3.10)
# project(imgengine C)

# set(CMAKE_C_STANDARD 11)

# # Optimization & Hardening Flags
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -Wall -Wextra -Werror")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -fno-plt -fPIC")

# # Standard Search Paths
# include_directories(
#     api/v1 
#     src/ 
#     include/ 
#     memory/ 
#     runtime/ 
#     arch/
#     pipeline/
#     plugins/
#     io/
# )

# # Find External Dependencies
# find_package(Threads REQUIRED)
# find_library(TURBOJPEG_LIB NAMES turbojpeg)

# # Collect all internal source files
# file(GLOB_RECURSE ENGINE_SOURCES 
#     "src/*.c" 
#     "arch/*.c" 
#     "pipeline/*.c" 
#     "plugins/*.c" 
#     "io/*.c" 
#     "runtime/*.c" 
#     "memory/*.c"
# )

# # 1. Generate the Shared Library (v1.0 API)
# add_library(imgengine SHARED ${ENGINE_SOURCES})
# target_link_libraries(imgengine PRIVATE Threads::Threads ${TURBOJPEG_LIB} m)

# # 2. Generate the CLI Executable
# add_executable(imgengine_cli cmd/imgengine/main.c cmd/imgengine/args.c)
# target_link_libraries(imgengine_cli PRIVATE imgengine)

# # 3. Performance Benchmarking Tool
# add_executable(bench_lat cmd/bench/lat_bench.c)
# target_link_libraries(bench_lat PRIVATE imgengine)


cmake_minimum_required(VERSION 3.10)
project(imgengine C)

set(CMAKE_C_STANDARD 11)

# --- 1. HARDENING & OPTIMIZATION FLAGS ---
# -march=native: Uses current CPU's SIMD (AVX2/AVX512)
# -fno-plt: Bypasses PLT for faster calls
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -fno-plt -fPIC")

# --- 2. ARCHITECTURE DETECTION (CRITICAL) ---
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
    set(ARCH_DIR "arch/x86_64")
    add_definitions(-DARCH_X86_64)
    message(STATUS "Build target: x86_64 (AVX-512/AVX2)")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(ARCH_DIR "arch/aarch64")
    add_definitions(-DARCH_AARCH64)
    message(STATUS "Build target: aarch64 (NEON)")
else()
    set(ARCH_DIR "arch/scalar")
    message(STATUS "Build target: Generic Scalar Fallback")
endif()

# --- 3. SEARCH PATHS ---
include_directories(
    .
    api/v1 
    src 
    include 
    memory 
    runtime 
    pipeline 
    plugins 
    io
)

# --- 4. EXTERNAL DEPENDENCIES ---
find_package(Threads REQUIRED)
# Reliable search for TurboJPEG on Ubuntu/Codespaces
find_path(TURBOJPEG_INCLUDE_DIR NAMES turbojpeg.h)
find_library(TURBOJPEG_LIB NAMES turbojpeg)

# if(NOT TURBOJPEG_LIB)
#     message(FATAL_ERROR "libturbojpeg not found. Run: sudo apt-get install libturbojpeg0-dev")
# endif()

# --- 5. SOURCE COLLECTION (Filtered by Arch) ---
file(GLOB_RECURSE ENGINE_SOURCES 
    "src/*.c" 
    "pipeline/*.c" 
    "plugins/*.c" 
    "io/*.c" 
    "runtime/*.c" 
    "memory/*.c"
    "${ARCH_DIR}/*.c" # Only pulls the compatible silicon logic
)

# --- 6. TARGETS ---

# A. The Core Shared Library
add_library(imgengine SHARED ${ENGINE_SOURCES})
target_include_directories(imgengine PRIVATE ${TURBOJPEG_INCLUDE_DIR})
target_link_libraries(imgengine PRIVATE Threads::Threads ${TURBOJPEG_LIB} m)

# B. The Production CLI Tool
add_executable(imgengine_cli cmd/imgengine/main.c cmd/imgengine/args.c)
target_link_libraries(imgengine_cli PRIVATE imgengine)

# C. The Latency Benchmarker
# Ensure this file exists: touch cmd/bench/lat_bench.c
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmd/bench/lat_bench.c")
    add_executable(bench_lat cmd/bench/lat_bench.c)
    target_link_libraries(bench_lat PRIVATE imgengine)
endif()
