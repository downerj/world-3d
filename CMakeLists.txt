cmake_minimum_required(VERSION 3.28)

project(world-3d)

# set(USE_WINDOWING_SYSTEM "GLFW" CACHE STRING "The underlying windowing system to use (GLFW, GLUT, SDL)")
# set(USE_STATIC_SHADERS False CACHE STRING "Set to True to include shaders directly in the source code, False to load dynamically")
set(USE_WINDOWING_SYSTEM "GLFW")
set(USE_STATIC_SHADERS False)

set(SOURCES
  src/camera.cxx
  src/game.cxx
  src/graphics-engine.cxx
  src/graphics-gl.cxx
  src/graphics-types.cxx
  src/io.cxx
  src/main.cxx
  src/models.cxx
  src/window-glfw.cxx
)

include_directories(include)

set(CXX_STANDARD 17)

if((MSVC AND $<CONFIG:Debug>) OR (CMAKE_BUILD_TYPE STREQUAL Debug))
  add_compile_definitions("DEBUG")
endif()

if(USE_WINDOWING_SYSTEM STREQUAL "GLFW")
  find_package(glfw3 3.3 REQUIRED)
  add_compile_definitions("USE_GLFW")
# elseif(USE_WINDOWING_SYSTEM STREQUAL "GLUT")
#   add_compile_definitions("USE_GLUT")
# elseif(USE_WINDOWING_SYSTEM STREQUAL "SDL")
#   add_compile_definitions("USE_SDL")
endif()

if(NOT USE_STATIC_SHADERS)
  file(COPY "${CMAKE_SOURCE_DIR}/res" DESTINATION "${CMAKE_BINARY_DIR}/")
endif()

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wunreachable-code)
endif()

add_executable(world-3d ${SOURCES})
target_link_libraries(world-3d glfw)
