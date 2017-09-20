C-Toy
====

C-Toy is an interactive C(99) coding environment based on [TCC](http://bellard.org/tcc/).

Small, simple, no bullshit. Write cross-platform C code and see the result immediately. No installation or compiler required, download (~2mb), unzip, run CToy and play. Ready for Windows 64 bit and MacOSX 64 bit (linux in progress). Ideal for games, image processing, teaching, prototyping...

Features
--------

* CToy: program update on file save (use any text editor)
* CToy_player: to publish your project (dynamic update disabled)
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

[- CToy 1.01 Win64](http://anael.maratis3d.com/ctoy/bin/CToy-1.01-WIN-x86_64.zip)<br>
[- CToy 1.01 MacOSX](http://anael.maratis3d.com/ctoy/bin/CToy-1.01-OSX_10.6-x86_64.zip)<br>
[- [all versions]](http://anael.maratis3d.com/ctoy/bin/)

Getting started
---------------

- Launch CToy
- Open src/main.c using your favorite text editor
- Start coding (samples included)
- Save your file(s) and see the result in realtime

<p><img src="http://anael.maratis3d.com/ctoy/doc/preview.gif" width="75%" height="75%"/></p>

Building CToy from sources (CMake)
----------------------------------

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
