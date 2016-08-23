CToy
====

CToy is a C(99) live-coding environment based on [TCC](http://bellard.org/tcc/).
Ready for Windows 64 bit and MacOSX 64 bit (linux in progress).

Features
--------

* Realtime live-coding on file save (use any text editor) + static player
* Cross-platform (simplified) libc
* CToy API for window managment, inputs...
* Image processing with [MaratisTCL](https://github.com/anael-seghezzi/Maratis-Tiny-C-library)
* OpenGLES-2 header-based simulation
* OpenAL
* Embedded libtcc
* Load C-symbols from dll or dylib files
* Compile your project natively using CMake (tool included)
* Emscripten compatible (tool in progress)

Building (CMake)
----------------

**Unix:**

    mkdir Build
    cd Build
    cmake -G "Unix Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin -DCMAKE_BUILD_TYPE=Release
    make
    make install

**Windows:**

    mkdir Build
    cd Build
    cmake -G "Visual Studio 11 Win64" ../ -DCMAKE_INSTALL_PREFIX=../bin

Getting started
---------------

- Launch CToy
- Open src/main.c using your favorite text editor
- Start coding (samples included)
- Save your file(s) and see the result in realtime
