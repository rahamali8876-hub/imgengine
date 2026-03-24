### BUILD COMMANDS

    🪟 Windows (MinGW / MSYS / VS Code)
    mkdir build
    cd build

    cmake ..
    cmake --build .

### Linux / WSL

    mkdir build
    cd build

    cmake ..
    make

### RUN YOUR ENGINE After build

    ./imgengine_cli

    or Windows:

    imgengine_cli.exe

### downlaod image form INTERNET

    curl -L -o input.jpg https://picsum.photos/800

### HOW TO RUN (INDUSTRY STYLE)

    ./imgengine_cli --input in.jpg --output out.png

### Your case (A4 passport grid)

    ./imgengine_cli \
    --input ../input.jpg \
    --output final_output.png \
    --cols 6 \
    --rows 2 \
    --gap 15 \
    --width 3.5 \
    --height 3.0 \
    --padding 20

### ✔ PRO PRINT MODE (recommended) WITH ( PDF )

    ./imgengine_cli \
    --input ../input.jpg \
    --output final_output.png \
    --cols 6 \
    --rows 2 \
    --gap 15 \
    --width 3.5 \
    --height 3.0 \
    --padding 20 \
    --bleed 10 \
    --crop-mark 25 \
    --crop-thick 2

    **** PDF **** 
    
        ./imgengine_cli \
    --input ../input.jpg \
    --output final_output.pdf \
    --cols 6 \
    --rows 2 \
    --gap 15 \
    --width 3.5 \
    --height 3.0 \
    --padding 20

### Help menu

    ./imgengine_cli --help

cd ..
rm -rf build
rm -rf build && mkdir build && cd build && cmake .. && make

rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
