### 📁 FOLDER STRUCTURE

    imgengine/
    │
    ├── CMakeLists.txt
    │
    ├── include/imgengine/
    │   ├── api.h
    │   ├── context.h
    │   ├── memory_pool.h
    │   ├── image.h
    │   ├── resize.h
    │   ├── crop.h
    │   ├── border.h
    │   └── layout.h
    │
    ├── src/
    │   ├── core/
    │   │     memory_pool.c
    │   │     image.c
    │   │     context.c
    │   │
    │   ├── ops/
    │   │     resize.c
    │   │     crop.c
    │   │     border.c
    │   │
    │   ├── layout/
    │   │     grid_layout.c
    │   │
    │   ├── io/
    │   │     stb_impl.c
    │   │
    │   └── api/
    │         api.c
    │
    ├── third_party/stb/
    │   ├── stb_image.h
    │   └── stb_image_write.h
    │
    └── cli/
        └── main.c

### FINAL ARCHITECTURE

    core/
    api.c

    layout/
    grid_layout.c

    plugins/
    plugin_registry.c
    bleed_plugin.c
    crop_plugin.c
    (future: cmyk, icc, pdf)

    pipeline/
    plugin_runner.c
