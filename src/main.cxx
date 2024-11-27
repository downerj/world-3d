#include <array>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define QUOTE(x) #x
#define STRING(x) QUOTE(x)
#ifdef DEBUG
#define DEBUG_LOG(s) do { std::cout << s; } while (false);
#define DEBUG_ERROR(s) do { std::cerr << s; } while (false);
#define DEBUG_LOG_LINE(s) do { std::cout << s << '\n'; } while (false);
#define DEBUG_ERROR_LINE(s) do { std::cerr << s << '\n'; } while (false);
#define DEBUG_LOG_NEWLINE() do { std::cout << '\n'; } while (false);
#define DEBUG_ERROR_NEWLINE() do { std::cerr << '\n'; } while (false);
#else
#define DEBUG_LOG(s)
#define DEBUG_ERROR(s)
#define DEBUG_LOG_LINE(s)
#define DEBUG_ERROR_LINE(s)
#define DEBUG_LOG_NEWLINE()
#define DEBUG_ERROR_NEWLINE()
#endif

#ifdef DEBUG
void errorCallbackGLFW(int /*error*/, const char* description) {
  DEBUG_ERROR_LINE("GLFW error: " << description);
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
  DEBUG_ERROR_LINE("GL error: " << message);
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
    DEBUG_ERROR_LINE("GLFW error: Initialization failed");
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
    DEBUG_LOG_LINE("GL extension GL_ARB_debug_output available");
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(debugMessageCallbackGL, nullptr /*userParam*/);
  } else {
    DEBUG_LOG_LINE("GL extension GL_ARB_debug_output unavailable");
  }
#endif
  DEBUG_LOG_LINE("C++ version: " << STRING(__cplusplus));
  DEBUG_LOG_LINE("Driver OpenGL version: " << glGetString(GL_VERSION));
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
    DEBUG_ERROR_LINE("GL program error: " << programLog);
    GLsizei vertexLogLength{};
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
    if (vertexLogLength > 0) {
      std::string vertexLog{};
      vertexLog.resize(vertexLogLength);
      glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
      DEBUG_ERROR_LINE("GL vertex shader error: " << vertexLog);
    }
    GLsizei fragmentLogLength{};
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
    if (fragmentLogLength > 0) {
      std::string fragmentLog{};
      fragmentLog.resize(fragmentLogLength);
      glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog.data());
      DEBUG_ERROR_LINE("GL fragment shader error: " << fragmentLog);
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
