C-Toy
====

C-Toy is an interactive C(99) coding environment based on [TCC](http://bellard.org/tcc/).

Small, simple, no bullshit. Write cross-platform C code and see the result immediately. No installation or compiler required, download (<4mb), unzip, run CToy and play. Ready for Windows, MacOSX and Linux. Ideal for prototyping, learning, teaching...

Features
--------

* CToy: program update on file save (use any text editor)
* CToy_player: to publish your project (dynamic update disabled)
* API for window managment, inputs, persistent memory...
* Image processing with [MaratisTCL](https://github.com/anael-seghezzi/Maratis-Tiny-C-library)
* OpenGLES-2
* OpenAL
* Portable pen-tablet support (Wacom, etc)
* Use C-symbols from native dynamic libraries (*.dll etc) : just copy libraries in your_ctoy_path/lib/
* Pre-built Dear-Imgui suport (https://github.com/ocornut/imgui)
* Can also compile your project with other compilers (CMake script for gcc, vs, mingw)
* Emscripten compatible

Download
--------

[- CToy 1.06 Win64](http://anael.maratis3d.com/ctoy/bin/CToy-1.06-WIN-x86_64.zip)<br>
[- CToy 1.05 MacOSX](http://anael.maratis3d.com/ctoy/bin/CToy-1.05-OSX_10.6-x86_64.zip)<br>
[- CToy 1.05 Linux64](http://anael.maratis3d.com/ctoy/bin/CToy-1.05-Linux64-x86_64.zip)<br>
[- [all versions]](http://anael.maratis3d.com/ctoy/bin/)

Requirement For Linux: OpenAL

Getting started
---------------

- Launch CToy
- Open src/main.c using your favorite text editor
- Start coding (samples included)
- Save your file(s) and see the result in realtime

<p><img src="http://anael.maratis3d.com/ctoy/doc/preview.gif" width="75%" height="75%"/></p>

Usage
-----

C-Toy expects a main file in src/main.c.<br>
But instead of the standad C "main" function, the entry points are "ctoy_begin", "ctoy_main_loop" and "ctoy_end".

The compulsory "Hello, World!" program is then (in src/main.c):<br>
```c
#include <ctoy.h> // ctoy API (including frequently used ANSI C libs)

void ctoy_begin() // called at the beginning of the program
{
   printf("Hello, World!\n");
}

void ctoy_main_loop() // called at every update of the main loop
{}

void ctoy_end() // called at the end of the program
{}
```
Every time you modify src/main.c or any other file connected to it (directly or recursively included), C-Toy will recompile and restart the program dynamically.

One other difference with standard C is the use of persistent memory to maintain a bloc of memory intact between recompiles.
For example:

```c
#include <ctoy.h>

void *persistent_memory = NULL;

void ctoy_begin()
{
   if (ctoy_t() == 0) {
      persistent_memory = calloc(256, 1); // allocate 256 bytes with zero value
      ctoy_register_memory(persistent_memory); // register persistent memory
   }
   else {
      persistent_memory = ctoy_retrieve_memory(); // retrieve persistent memory
   }
}

void ctoy_main_loop()
{
   int *persistent_counter = (int *)persistent_memory; // access a piece of persistent memory
   (*persistent_counter)++; // do something with the data
   printf("persistent_counter = %d\n", (*persistent_counter)); // print the content
}

void ctoy_end()
{}
```
(You can store any data that was manually allocated with malloc, it can be an array or a global pointer. Just avoid storing function pointers, as functions addresses may change after recompiles, or update them after calling ctoy_retrieve_memory)

Documentation
-------------

C-Toy API: https://github.com/anael-seghezzi/CToy/blob/master/ressources/include/ctoy.h<br>
MaratisTCL: https://github.com/anael-seghezzi/Maratis-Tiny-C-library<br>
OpenGLES2: https://www.khronos.org/registry/OpenGL-Refpages/es2.0/<br>
OpenAL: https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf<br>

Tutorials and examples
----------------------

Wiki: https://github.com/anael-seghezzi/CToy/wiki

License
-------

CToy is licensed under the zlib/libpng License.

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
