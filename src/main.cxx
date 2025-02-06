#include <array>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "debug.hxx"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifdef DEBUG
void errorCallbackGLFW(int /*error*/, const char* description) {
  LOG_ERROR("GLFW error: " << description << '\n');
}

void debugMessageCallbackGL(
  GLenum /*source*/,
  GLenum /*type*/,
  GLuint /*id*/,
  GLenum /*severity*/,
  GLsizei /*length*/,
  const GLchar* message,
  const void* /*userParam*/
) {
  LOG_ERROR("GL error: " << message << '\n');
}
#endif

void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
  const int keyUp{action & GLFW_KEY_UP};
  const bool keyQ{key == GLFW_KEY_Q};
  const bool keyW{key == GLFW_KEY_W};
  const bool keyF4{key == GLFW_KEY_F4};
  const int keyCtrl{mods & GLFW_MOD_CONTROL};
  const int keyAlt{mods & GLFW_MOD_ALT};
  if (keyUp && ((keyCtrl && (keyQ || keyW)) || (keyAlt && keyF4))) {
    glfwSetWindowShouldClose(window, true);
    return;
  }
}

std::string readFile(const char* const fileName) {
  std::ifstream streamIn{fileName};
  std::ostringstream streamOut{};
  std::string s{};
  while (std::getline(streamIn, s)) {
    streamOut << s << '\n';
  }
  return streamOut.str();
}

constexpr std::tuple<int, int> windowSize{640, 480};
constexpr std::tuple<int, int> versionOpenGL{3, 3};

GLFWwindow* initializeWindow() {
  if (!glfwInit()) {
    LOG_ERROR("GLFW error: Initialization failed\n");
    return nullptr;
  }
#ifdef DEBUG
  glfwSetErrorCallback(errorCallbackGLFW);
#endif
  const auto [windowWidth, windowHeight]{windowSize};
  const char* windowTitle{"3D Flying Camera Test"};
  const auto [glMajor, glMinor]{versionOpenGL};
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, true);
#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
  GLFWwindow* window{glfwCreateWindow(
    windowWidth,
    windowHeight,
    windowTitle,
    nullptr /*monitor*/,
    nullptr /*share*/
  )};
  if (!window) {
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);
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
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, keyCallback);
  return window;
}

GLuint createShader(GLenum type, const std::string& source) {
  GLuint shader{glCreateShader(type)};
  std::array<const char*, 1> sources{source.data()};
  glShaderSource(shader, 1, sources.data(), nullptr /*length*/);
  glCompileShader(shader);
  return shader;
}

GLuint createProgram(const std::string& vertexSource, const std::string& fragmentSource) {
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

ProgramData initializeGL() {
  // TODO: Implement std::filesystem calls to check for shader file existence.
  std::string vertexSource{readFile("res/shaders/main.vert")};
  std::string fragmentSource{readFile("res/shaders/main.frag")};
  GLuint program{createProgram(vertexSource, fragmentSource)};
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  return ProgramData{program, vao, 3};
}

void mainLoop(GLFWwindow* window, const ProgramData& programData) {
  while (!glfwWindowShouldClose(window)) {
    int width{};
    int height{};
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.f, .5f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programData.program);
    // OpenGL Core (3.2+) requires explicit binding of a VAO before drawing,
    // even if no vertex data is being provided to the shaders.
    glBindVertexArray(programData.vao);
    glDrawArrays(GL_TRIANGLES, 0, programData.vertexCount);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void cleanUp(GLFWwindow* window, ProgramData& programData) {
  glfwDestroyWindow(window);
  glDeleteVertexArrays(1, &programData.vao);
  glfwTerminate();
}

int main(int /*argc*/, char** /*argv*/) {
  GLFWwindow* window{initializeWindow()};
  if (window == nullptr) {
    std::exit(EXIT_FAILURE);
  }
  ProgramData programData{initializeGL()};
  mainLoop(window, programData);
  cleanUp(window, programData);
}
