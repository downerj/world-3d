# world-3d
This project is meant to be a personal exploration into modern OpenGL (Core).

## Building
This version of the software successfully builds with debug flags on Ubuntu Linux 24.10.

### CMake
This project uses the CMake metabuild system. Consider using either the CMake Curses command-line utility (`ccmake`) or the CMake Qt GUI.

### Third-party libraries

#### GLAD 2
This project utilizes the GLAD extension loading library for OpenGL. You'll first need to generate a GLAD header file for inclusion in the project using [GLAD 2](https://gen.glad.sh).

The GLAD settings are as follows:
- **Generator**: C/C++
- **APIs**:
  - **gl**: Version 3.3 Core
- **Extensions**:
  - GL_ARB_debug_output
- **Options**:
  - Header only

You can also use [the corresponding permalink](http://glad.sh/#api=gl%3Acore%3D3.3&extensions=GL_ARB_debug_output&generator=c&options=HEADER_ONLY).

#### GLFW
This project uses the GLFW window and context management library. It is linked dynamically.

### Ubuntu Linux (terminal)
1. Install/generate the following:
   - CMake
   - [Optional] CMake Curses or CMake Qt GUI
   - GCC
   - GNU Make
   - GLAD 2 header
   - GLFW &#x2265;3.3
   - GLM &#x2265;0.9.9
1. Note the location of the source directory (i.e. your clone of this repository on disk).
1. Copy the GLAD header file into the following (new) location:
   - `<source directory>/include/glad/gl.h`
1. In a terminal, create the build directory (if building for the first time).
   - Note that it's recommended to create an [out-of-source build](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html#directory-structure).
   - Example: `$HOME/world-3d.build/debug`
1. Navigate/`cd` to the build directory.
1. Run CMake.
   - Using CMake CLI: `cmake -B ./ -S <source directory> [Options]`
      - Options include:
         - `-DCMAKE_BUILD_TYPE=Debug` for a debug build; omit for a release build
         - `-Dglfw3_DIR=<GLFW directory>` for the path to the installed GLFW 3 library
   - Using CMake Curses:
      - `ccmake -B ./ -S <source directory>`
      - Set each option as needed.
      - Type `C` to configure.
      - Type `G` to generate.
   - Using the CMake Qt GUI (_more details to come_).
1. Compile using `make`.
1. Run using `./world-3d`.
