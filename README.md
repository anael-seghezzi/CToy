CToy
====

CToy is a C(99) live-coding environment based on [TCC](http://bellard.org/tcc/).

Small, realtime, no bullshit. Write cross-platform-checked C code dynamically, compatible with standard compilers and Emscripten (web deployment). Ready for Windows 64 bit and MacOSX 64 bit (linux in progress).

Features
--------

* Live-coding on file save (use any text editor) + static player
* CToy API for window managment, inputs, persistent memory...
* Image processing with [MaratisTCL](https://github.com/anael-seghezzi/Maratis-Tiny-C-library)
* OpenGLES-2 header-based simulation
* OpenAL
* Embedded libtcc
* Load C-symbols from dll or dylib files
* Compile your project natively using CMake (tool included)
* Emscripten compatible (tool in progress)

Download
--------

[CToy 1.00 MacOSX](http://anael.maratis3d.com/ctoy/bin/CToy-1.00-OSX_10.6-x86_64.zip)

Getting started
---------------

- Launch CToy
- Open src/main.c using your favorite text editor
- Start coding (samples included)
- Save your file(s) and see the result in realtime

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
