#include <array>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "debug.hxx"
#include "io.hxx"
#include "window.hxx"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum /*source*/,
  GLenum /*type*/,
  GLuint /*id*/,
  GLenum /*severity*/,
  GLsizei /*length*/,
  const GLchar* message,
  const void* /*userParam*/
) -> void {
  LOG_ERROR("GL error: " << message << '\n');
}
#endif // DEBUG

auto createShader(GLenum type, const std::string& source) -> GLuint {
  GLuint shader{glCreateShader(type)};
  std::array<const char*, 1> sources{source.data()};
  glShaderSource(shader, 1, sources.data(), nullptr /*length*/);
  glCompileShader(shader);
  return shader;
}

auto createProgram(
  const std::string& vertexSource, const std::string& fragmentSource
) -> GLuint {
  GLuint vertexShader{createShader(GL_VERTEX_SHADER, vertexSource)};
  GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentSource)};
  GLuint program{glCreateProgram()};
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  GLint status{};
  glGetProgramiv(program, GL_LINK_STATUS, &status);
#ifdef DEBUG
  if (!status) {
    GLsizei programLogLength{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
    std::string programLog{};
    programLog.resize(programLogLength);
    glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog.data());
    LOG_ERROR("GL program error: " << programLog << '\n');
    GLsizei vertexLogLength{};
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
    if (vertexLogLength > 0) {
      std::string vertexLog{};
      vertexLog.resize(vertexLogLength);
      glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
      LOG_ERROR("GL vertex shader error: " << vertexLog << '\n');
    }
    GLsizei fragmentLogLength{};
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
    if (fragmentLogLength > 0) {
      std::string fragmentLog{};
      fragmentLog.resize(fragmentLogLength);
      glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog.data());
      LOG_ERROR("GL fragment shader error: " << fragmentLog << '\n');
    }
  }
#endif
  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  if (!status) {
    throw std::runtime_error{"Error creating GL program"};
  }
  return program;
}

struct ProgramData {
  GLuint program;
  GLuint vao;
  GLsizei vertexCount;

  ProgramData() = delete;
  ProgramData(GLuint program, GLuint vao, GLsizei vertexCount) :
    program{program}, vao{vao}, vertexCount{vertexCount} {}
};

auto initializeGraphics() -> ProgramData {
    gladLoadGL(glfwGetProcAddress);
#ifdef DEBUG
  if (GLAD_GL_ARB_debug_output) {
    LOG("GL extension GL_ARB_debug_output available\n");
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(debugMessageCallbackGL, nullptr /*userParam*/);
  } else {
    LOG("GL extension GL_ARB_debug_output unavailable\n");
  }
#endif
  LOG("C++ version: " << STRING(__cplusplus) << '\n');
  LOG("Driver OpenGL version: " << glGetString(GL_VERSION) << '\n');

  // TODO: Implement std::filesystem calls to check for shader file existence.
  std::string vertexSource{readFile("res/shaders/main.vert")};
  std::string fragmentSource{readFile("res/shaders/main.frag")};
  GLuint program{createProgram(vertexSource, fragmentSource)};
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  return ProgramData{program, vao, 3};
}

auto render(WindowHandler& window, const ProgramData& programData) -> void {
  glViewport(0, 0, window.getWidth(), window.getHeight());
  glClearColor(0.f, .5f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(programData.program);
  // OpenGL Core (3.2+) requires explicit binding of a VAO before drawing,
  // even if no vertex data is being provided to the shaders.
  glBindVertexArray(programData.vao);
  glDrawArrays(GL_TRIANGLES, 0, programData.vertexCount);
}

auto cleanUp(ProgramData& programData) -> void {
  glDeleteVertexArrays(1, &programData.vao);
}

auto main(int /*argc*/, char** /*argv*/) -> int {
  try {
    WindowHandler window{};
    ProgramData programData{initializeGraphics()};
    while (window.isActive()) {
      const WindowActions& actions{window.getActions()};
      if (actions.close) {
        window.close();
        break;
      }
      if (actions.resetSize) {
        window.resetSize();
      }
      window.resetActions();
      window.preRender();
      render(window, programData);
      window.postRender();
    }
    cleanUp(programData);
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}
