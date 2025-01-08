# Brake cascade

Display & process point cloud on ARM processor.


### Tested on

| Soc  | CPU  | GPU  | OS  |
|---|---|---|---|
|  Raspberry PI 4 | ARM-72 Cortex  | Broadcom Vidcore  | Raspian
|  HP Zbook | Core i7  | NVidia  | Windows


![1-20-2024_lanes_norm,wet,icy](https://github.com/kerm1t/brake_cascade/assets/26294323/7ad6ec16-5e65-4118-9924-05db2c54d6c9)


How to build
------
### Linux / Raspian


1. install libs
``` sudo apt-get install libsdl2-dev libglew-dev ```
2. build
``` g++ -g -std=c++17 main.cpp -o openglengine -lGL -lSDL2 -lGLEW```


### Windows

1. get SDL2-devel, e.g. from https://github.com/libsdl-org/SDL/releases
2. get Glew, e.g. https://sourceforge.net/projects/glew/files/glew/2.1.0/
3. update cmake to use above libs/includes
4. run cmake
5. copy SDL.dll from /lib\SDL2-2.26.4\lib\x64 to /Debug or /Release
