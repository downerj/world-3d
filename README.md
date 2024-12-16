# flying-camera-test-gl
This project is a 3D camera test in OpenGL. It is meant to be a personal exploration into modern OpenGL (Core).

## Building
This version of the software successfully builds with debug flags on Ubuntu Linux 24.10 and Windows 11.

### Ubuntu Linux
1. Install/generate the following:
   - GCC
   - GNU Make
   - GLAD 2
     - OpenGL 3.3 Core
     - `GL_ARB_debug_output` extension
     - Header only
     - Command line: `--api='gl:core=3.3' --extensions='GL_ARB_debug_output' c --header-only`
     - Online: http://glad.sh/#api=gl%3Acore%3D3.3&extensions=GL_ARB_debug_output&generator=c&options=HEADER_ONLY
   - GLFW 3.4 (dynamically linked)
   - GLM 0.9.9.8
2. Copy the GLAD header file into a new `include/glad` directory:
   - `include/glad/gl.h`
3. Compile using `make`.

### Windows (Visual Studio)
1. Install/generate the following:
   - GLAD 2
     - OpenGL 3.3 Core
     - `GL_ARB_debug_output` extension
     - Header only
     - Command line: `--api='gl:core=3.3' --extensions='GL_ARB_debug_output' c --header-only`
     - Online: http://glad.sh/#api=gl%3Acore%3D3.3&extensions=GL_ARB_debug_output&generator=c&options=HEADER_ONLY
   - GLFW 3.4 (dynamically linked)
   - GLM 1.0.1
2. Copy the GLAD, GLFW and GLM files into their corresponding new directories:
   - `include/glad/gl.h`
   - `include/GLFW/glfw3.h`
   - `include/glm/**/*`
   - `lib/glfw3.dll`
   - `lib/glfw3.lib`
3. Build using **Build** > **Build Solution**.