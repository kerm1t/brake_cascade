# pointcloud4ARM

Display & process point cloud on ARM processor.


### Tested on

| Soc  | CPU  | GPU  | OS  |   |
|---|---|---|---|---|
|  Raspberry PI 4 | ARM-72 Cortex  | Broadcom Vidcore  | Raspian  |  ![image](https://github-am.geo.conti.de/storage/user/606/files/f6a594e2-8c31-441d-a322-f3465065ecb9)
|  HP Zbook | Core i7  | NVidia  | Windows  |  ![image](https://github-am.geo.conti.de/storage/user/606/files/39923e09-1c32-4352-9fb8-f8ec543175e0)

![image](https://github-am.geo.conti.de/storage/user/606/files/ab8a22bf-c790-452d-9dcb-b83d3a02cd22)


How to build
------
### Linux / Raspian


1. install libs
``` sudo apt-get install libsdl2-dev libglew-dev ```
2. build
``` g++ -g -std=c++17 main.cpp -o openglengine -lGL -lSDL2 -lGLEW```


### Windows

1. get SDL2
2. get Glew
3. run cmake
4. copy SDL.dll from /lib\SDL2-2.26.4\lib\x64 to /Debug or /Release