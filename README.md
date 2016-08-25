CToy
====

CToy is a C(99) live-coding environment based on [TCC](http://bellard.org/tcc/).

Small, simple, no bullshit. Write standard cross-platform code and see the result immediately. No installation required, download (~2mb), run CToy and play. Ready for Windows 64 bit and MacOSX 64 bit (linux in progress). Ideal for games, image processing, teaching, or anything C can do.

Features
--------

* CToy: live-coding on file save (use any text editor)
* CToy_player: to publish your project (live update disabled)
* API for window managment, inputs, persistent memory...
* Image processing with [MaratisTCL](https://github.com/anael-seghezzi/Maratis-Tiny-C-library)
* OpenGLES-2 simulation
* OpenAL
* Embedded libtcc
* Plug external libraries: use C-symbols from dll or dylib files
* Can also compile your project with other compilers (CMake script for gcc, vs, mingw)
* Emscripten compatible (tool in progress)

Download
--------

[CToy 1.00 Win64](http://anael.maratis3d.com/ctoy/bin/CToy-1.00-WIN-x86_64.zip)<br>
[CToy 1.00 MacOSX](http://anael.maratis3d.com/ctoy/bin/CToy-1.00-OSX_10.6-x86_64.zip)

Getting started
---------------

- Launch CToy
- Open src/main.c using your favorite text editor
- Start coding (samples included)
- Save your file(s) and see the result in realtime

<p><img src="http://anael.maratis3d.com/ctoy/doc/preview.gif" width="75%" height="75%"/></p>

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


(libtcc.dll and libtcc.dylib where pre-built from a fork of tcc:
[libtcc-fork](https://github.com/anael-seghezzi/tcc-0.9.26))
