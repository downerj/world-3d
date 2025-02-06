# world-3d
This project is meant to be a personal exploration into modern OpenGL (Core).

## Building
This version of the software successfully builds with debug flags on Ubuntu Linux 24.10.

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
This project uses the GLFW window and context management library. It can be linked dynamically or statically, though static linking may require you to build GLFW from source.

### Ubuntu Linux
1. Install/generate the following:
   - GCC
   - GNU Make
   - GLAD 2 header
   - GLFW 3.4
   - GLM 0.9.9.8
1. Copy the GLAD header file into the following (new) location:
   - `include/glad/gl.h`
1. If you built GLFW from source (e.g. for static linking), copy the header and library files into the following (new) locations:
   - `include/GLFW/glfw3.h`
   - `lib/libglfw3.a`
1. Set/`export` the following environment variables, as needed:
   - For static linking, set `STATIC_BUILD=1`.
   - For debug builds, set `DEBUG_BUILD=1`.
1. Compile using `make`.
1. Run the app using `bin/release/world-3d` (or `bin/debug/world-3d` if `DEBUG_BUILD=1` was set).
