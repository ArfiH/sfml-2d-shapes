# Assignment1

A 2D game built with:

* C++23
* SFML 3
* Dear ImGui
* ImGui-SFML
* CMake

## Build Requirements

* MSYS2 UCRT64
* GCC 16+
* CMake 3.20+

## Build

```bash
mkdir build
cd build

cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64

cmake --build .
```

## Run

```bash
Assignment1.exe
```
