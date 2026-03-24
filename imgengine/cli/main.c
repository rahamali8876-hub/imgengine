// // src/cli/main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "imgengine/api.h"
#include "imgengine/context.h"

#define DEFAULT_DPI 300
#define DEFAULT_COLS 2
#define DEFAULT_ROWS 3
#define DEFAULT_GAP 15
#define DEFAULT_BORDER 2
#define DEFAULT_W_CM 4.5f
#define DEFAULT_H_CM 3.5f
#define DEFAULT_PADDING 20

// 🔥 NEW DEFAULTS (FIXED SYNTAX)
#define DEFAULT_CROP_MARK 20
#define DEFAULT_CROP_THICKNESS 2

#define DEFAULT_BLEED 10
#define DEFAULT_CROP_OFFSET 8

static void print_usage()
{
    printf("\n");
    printf("IMGENGINE CLI\n");
    printf("Professional Photo Layout & Print Pipeline\n");
    printf("------------------------------------------------------------\n\n");

    printf("USAGE:\n");
    printf("  imgengine_cli --input <file> [--output <file>] [OPTIONS]\n\n");

    printf("DESCRIPTION:\n");
    printf("  Generate print-ready photo sheets (passport, ID, studio prints)\n");
    printf("  with automatic layout, scaling, bleed, and crop marks.\n\n");

    printf("SUPPORTED FORMATS:\n");
    printf("  Input  : JPG, PNG\n");
    printf("  Output : PNG, PDF (auto-detected by extension)\n\n");

    printf("REQUIRED:\n");
    printf("  --input <file>         Input image (jpg/png)\n\n");
    printf("OUTPUT:\n");
    printf("  --output <file>        Output file (png/pdf) (default: output.png)\n\n");

    printf("LAYOUT OPTIONS:\n");
    printf("  --cols <int>           Number of columns (default: %d)\n", DEFAULT_COLS);
    printf("  --rows <int>           Number of rows (default: %d)\n", DEFAULT_ROWS);
    printf("  --gap <px>             Gap between photos in pixels (default: %d)\n", DEFAULT_GAP);
    printf("  --padding <px>         Page margin in pixels (default: %d)\n", DEFAULT_PADDING);
    printf("\n");

    printf("PHOTO SETTINGS:\n");
    printf("  --width <cm>           Photo width in cm (default: %.1f)\n", DEFAULT_W_CM);
    printf("  --height <cm>          Photo height in cm (default: %.1f)\n", DEFAULT_H_CM);
    printf("  --dpi <int>            Print DPI (default: %d)\n", DEFAULT_DPI);
    printf("  --border <px>          Border thickness (default: %d)\n", DEFAULT_BORDER);
    printf("\n");

    printf("PRINT / PRO FEATURES:\n");
    printf("  --bleed <px>           Bleed size (prevents white edges) (default: %d)\n", DEFAULT_BLEED);
    printf("  --crop-mark <px>       Crop mark length (default: %d)\n", DEFAULT_CROP_MARK);
    printf("  --crop-thickness <px>  Crop mark thickness (default: %d)\n", DEFAULT_CROP_THICKNESS);
    printf("  --crop-offset <px>     Distance of crop marks from image edge (default: %d)\n", DEFAULT_CROP_OFFSET);
    printf("\n");

    printf("AUTO BEHAVIOR:\n");
    printf("  ✔ Automatically scales layout to fit page\n");
    printf("  ✔ Keeps aspect ratio intact\n");
    printf("  ✔ Centers horizontally, aligns top for print cutting\n");
    printf("\n");

    printf("EXAMPLES:\n\n");

    printf("  1) Basic passport sheet:\n");
    printf("     imgengine_cli --input in.jpg --cols 3 --rows 2\n\n");

    printf("  2) Studio A4 layout:\n");
    printf("     imgengine_cli --input in.jpg --output out.png \\\n");
    printf("         --cols 6 --rows 2 --gap 15 --padding 20 \\\n");
    printf("         --width 3.5 --height 3.0\n\n");

    printf("  3) Print-ready (bleed + crop marks):\n");
    printf("     imgengine_cli --input in.jpg --output out.png \\\n");
    printf("         --cols 6 --rows 2 --bleed 10 \\\n");
    printf("         --crop-mark 25 --crop-offset 8\n\n");

    printf("  4) PDF export (print shop):\n");
    printf("     imgengine_cli --input in.jpg --output out.pdf \\\n");
    printf("         --cols 6 --rows 2 --bleed 10\n\n");

    printf("NOTES:\n");
    printf("  • Use 300 DPI for professional printing\n");
    printf("  • Bleed is required for edge-safe cutting\n");
    printf("  • Crop marks are aligned per photo (lab standard)\n");
    printf("\n");

    printf("For documentation and updates, visit: https://github.com/Rofikali/imgengine/tree/main/imgengine\n\n");
}

int main(int argc, char **argv)
{
    const char *input = NULL;
    const char *output = "output.png";

    int cols = DEFAULT_COLS;
    int rows = DEFAULT_ROWS;
    int gap = DEFAULT_GAP;
    int dpi = DEFAULT_DPI;
    int border = DEFAULT_BORDER;
    int padding = DEFAULT_PADDING;

    int verbose = 1;

    int crop_mark = DEFAULT_CROP_MARK;
    int crop_thickness = DEFAULT_CROP_THICKNESS;
    int bleed = DEFAULT_BLEED;
    int crop_offset = DEFAULT_CROP_OFFSET;

    float w_cm = DEFAULT_W_CM;
    float h_cm = DEFAULT_H_CM;

    if (argc == 1)
    {
        print_usage();
        return 0;
    }

    // =========================
    // ARG PARSER
    // =========================
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--input") == 0 && i + 1 < argc)
            input = argv[++i];

        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
            output = argv[++i];

        else if (strcmp(argv[i], "--cols") == 0 && i + 1 < argc)
            cols = atoi(argv[++i]);

        else if (strcmp(argv[i], "--rows") == 0 && i + 1 < argc)
            rows = atoi(argv[++i]);

        else if (strcmp(argv[i], "--gap") == 0 && i + 1 < argc)
            gap = atoi(argv[++i]);

        else if (strcmp(argv[i], "--dpi") == 0 && i + 1 < argc)
            dpi = atoi(argv[++i]);

        else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc)
            w_cm = atof(argv[++i]);

        else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc)
            h_cm = atof(argv[++i]);

        else if (strcmp(argv[i], "--border") == 0 && i + 1 < argc)
            border = atoi(argv[++i]);

        else if (strcmp(argv[i], "--padding") == 0 && i + 1 < argc)
            padding = atoi(argv[++i]);

        else if (strcmp(argv[i], "--crop-mark") == 0 && i + 1 < argc)
            crop_mark = atoi(argv[++i]);

        else if (strcmp(argv[i], "--crop-thickness") == 0 && i + 1 < argc)
            crop_thickness = atoi(argv[++i]);

        else if (strcmp(argv[i], "--bleed") == 0 && i + 1 < argc)
            bleed = atoi(argv[++i]);

        else if (strcmp(argv[i], "--crop-offset") == 0 && i + 1 < argc)
            crop_offset = atoi(argv[++i]);

        else if (strcmp(argv[i], "--quiet") == 0)
            verbose = 0;

        else if (strcmp(argv[i], "--help") == 0)
        {
            print_usage();
            return 0;
        }

        else
        {
            printf("❌ Unknown argument: %s\n\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    // =========================
    // VALIDATION (AFTER PARSE)
    // =========================
    if (!input)
    {
        printf("❌ Error: --input is required\n");
        return 1;
    }

    if (cols <= 0 || rows <= 0)
    {
        printf("❌ Error: cols and rows must be > 0\n");
        return 1;
    }

    if (dpi <= 0)
    {
        printf("❌ Error: dpi must be > 0\n");
        return 1;
    }

    if (w_cm <= 0 || h_cm <= 0)
    {
        printf("❌ Error: width/height must be > 0\n");
        return 1;
    }

    // =========================
    // INIT
    // =========================
    img_ctx_t ctx;
    img_ctx_init(&ctx, 100 * 1024 * 1024);

    img_job_t job = {
        .photo_w_cm = w_cm,
        .photo_h_cm = h_cm,
        .dpi = dpi,
        .cols = cols,
        .rows = rows,
        .gap = gap,
        .border_px = border,
        .padding = padding,

        .crop_mark_px = crop_mark,
        .crop_thickness = crop_thickness,
        .bleed_px = bleed,
        .crop_offset_px = crop_offset,

        .mode = IMG_FILL,
        .bg_r = 255,
        .bg_g = 255,
        .bg_b = 255};

    if (verbose)
    {
        printf("Layout: %dx%d | size=%.1fx%.1f cm | gap=%d padding=%d\n",
               cols, rows, w_cm, h_cm, gap, padding);
    }

    // =========================
    // RUN ENGINE
    // =========================
    int ret = imgengine_run(&ctx, input, output, &job);

    if (ret != 0)
        printf("❌ Failed with error code: %d\n", ret);
    else if (verbose)
        printf("✅ Output generated: %s\n", output);

    img_ctx_destroy(&ctx);
    return ret;
}

// int main(int argc, char **argv)
// {
//     const char *input = NULL;
//     const char *output = "output.png";

//     int cols = DEFAULT_COLS;
//     int rows = DEFAULT_ROWS;
//     int gap = DEFAULT_GAP;
//     int dpi = DEFAULT_DPI;
//     int border = DEFAULT_BORDER;
//     int padding = DEFAULT_PADDING;

//     // Add QUIET / VERBOSE (pro CLI feature)
//     int verbose = 1;

//     // CROP / BLEED
//     int crop_mark = DEFAULT_CROP_MARK;
//     int crop_thickness = DEFAULT_CROP_THICKNESS;
//     int bleed = DEFAULT_BLEED;
//     int crop_offset = DEFAULT_CROP_OFFSET;

//     float w_cm = DEFAULT_W_CM;
//     float h_cm = DEFAULT_H_CM;

//     if (cols <= 0 || rows <= 0)
//     {
//         printf("❌ Error: cols and rows must be > 0\n");
//         return 1;
//     }

//     if (dpi <= 0)
//     {
//         printf("❌ Error: dpi must be > 0\n");
//         return 1;
//     }

//     if (w_cm <= 0 || h_cm <= 0)
//     {
//         printf("❌ Error: width/height must be > 0\n");
//         return 1;
//     }

//     if (argc == 1)
//     {
//         print_usage();
//         return 0;
//     }

//     // =========================
//     // ARG PARSER
//     // =========================
//     for (int i = 1; i < argc; i++)
//     {
//         if (strcmp(argv[i], "--input") == 0 && i + 1 < argc)
//             input = argv[++i];

//         else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
//             output = argv[++i];

//         else if (strcmp(argv[i], "--cols") == 0 && i + 1 < argc)
//             cols = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--rows") == 0 && i + 1 < argc)
//             rows = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--gap") == 0 && i + 1 < argc)
//             gap = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--dpi") == 0 && i + 1 < argc)
//             dpi = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc)
//             w_cm = atof(argv[++i]);

//         else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc)
//             h_cm = atof(argv[++i]);

//         else if (strcmp(argv[i], "--border") == 0 && i + 1 < argc)
//             border = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--padding") == 0 && i + 1 < argc)
//             padding = atoi(argv[++i]);

//         // 🔥 NEW FLAGS

//         else if (strcmp(argv[i], "--crop-mark") == 0 && i + 1 < argc)
//             crop_mark = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--crop-thickness") == 0 && i + 1 < argc)
//             crop_thickness = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--bleed") == 0 && i + 1 < argc)
//             bleed = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--crop-offset") == 0 && i + 1 < argc)
//             crop_offset = atoi(argv[++i]);

//         else if (strcmp(argv[i], "--quiet") == 0)
//             verbose = 0;

//         if (verbose)
//             printf("Layout: %dx%d ...\n");
//         else if (strcmp(argv[i], "--help") == 0)
//         {

//             printf("PERFORMANCE:\n");
//             printf("  • Uses memory pool for fast allocation\n");
//             printf("  • SIMD optimized (AVX2) image processing\n");
//             printf("  • Designed for batch print workflows\n\n");

//             printf("EXIT CODES:\n");
//             printf("  0   Success\n");
//             printf("  1   Invalid arguments\n");
//             printf("  2   File I/O error\n");
//             printf("  3   Memory allocation failure\n\n");
//             print_usage();
//             return 0;
//         }

//         else
//         {
//             printf("❌ Unknown argument: %s\n", argv[i]);
//             print_usage();
//             return 1;
//         }
//     }

//     if (!input)
//     {
//         printf("❌ Error: --input is required\n");
//         return 1;
//     }

//     // printf("Loading image: %s\n", input);

//     img_ctx_t ctx;
//     img_ctx_init(&ctx, 100 * 1024 * 1024);

//     img_job_t job = {
//         .photo_w_cm = w_cm,
//         .photo_h_cm = h_cm,
//         .dpi = dpi,
//         .cols = cols,
//         .rows = rows,
//         .gap = gap,
//         .border_px = border,
//         .padding = padding,

//         // 🔥 NEW

//         .crop_mark_px = crop_mark,
//         .crop_thickness = crop_thickness,
//         .bleed_px = bleed,
//         .crop_offset_px = crop_offset,

//         .mode = IMG_FILL,
//         .bg_r = 255,
//         .bg_g = 255,
//         .bg_b = 255};

//     printf("Layout: %dx%d | size=%.1fx%.1f cm | gap=%d padding=%d\n",
//            cols, rows, w_cm, h_cm, gap, padding);

//     int ret = imgengine_run(&ctx, input, output, &job);

//     if (ret != 0)
//         printf("❌ Failed with error code: %d\n", ret);
//     else
//         printf("✅ Output generated: %s\n", output);

//     img_ctx_destroy(&ctx);
//     return ret;
// }
