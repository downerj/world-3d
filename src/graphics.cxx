#include "graphics.hxx"

#include <array>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "debug.hxx"
#include "io.hxx"
#include "models.hxx"

/*
 * Declarations.
 */

namespace {

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar* message, const void* userParam
) -> void;
#endif // DEBUG
auto initializeGL() -> bool;

} // namespace

/*
 * Definitions.
 */

my::GraphicsEngine::GraphicsEngine() {
  if (!initializeGL()) {
    throw std::runtime_error{"Failed to initialize OpenGL"};
  }

  const std::unique_ptr<Geometry> geometry{std::make_unique<BasicTriangle>()};
  std::optional<std::string> vertexSource{readFile(
    "res/shaders/main.vert"
  )};
  std::optional<std::string> fragmentSource{readFile(
    "res/shaders/main.frag"
  )};
  if (!vertexSource || !fragmentSource) {
    throw std::runtime_error{"Failed to load shader sources"};
  }
  Shader vertexShader{ShaderType::Vertex, *vertexSource};
  Shader fragmentShader{ShaderType::Fragment, *fragmentSource};
  _programs.reserve(1);
  ShaderProgram& program{_programs.emplace_back(vertexShader, fragmentShader)};
  _buffers.reserve(3);
  const Buffer& positionBuffer{_buffers.emplace_back(
    BufferTarget::Array, geometry->getVertices(),
    geometry->getVertexMemorySize()
  )};
  const Buffer& colorBuffer{_buffers.emplace_back(
    BufferTarget::Array, geometry->getColors(),
    geometry->getColorMemorySize()
  )};
  const Buffer& indexBuffer{_buffers.emplace_back(
    BufferTarget::ElementArray, geometry->getIndices(),
    geometry->getIndexMemorySize()
  )};
  std::vector<ShaderAttribute> attributes{{
    "position", positionBuffer, geometry->getVertexCount(),
    AttributeType::Float, false, 0, nullptr
  }, {
    "color", colorBuffer, geometry->getColorCount(),
    AttributeType::Float, false, 0, nullptr
  }};
  program.emplaceVertexArray(
    program, attributes, indexBuffer,
    geometry->getIndexCount()
  );
  vertexShader.cleanup();
  fragmentShader.cleanup();
}

my::GraphicsEngine::~GraphicsEngine() {
  for (auto& program : _programs) {
    program.cleanup();
  }
  for (auto& buffer : _buffers) {
    buffer.cleanup();
  }
}

auto my::GraphicsEngine::resize(int width, int height) -> void {
  _windowWidth = width;
  _windowHeight = height;
}

auto my::GraphicsEngine::render() -> void {
  glViewport(0, 0, _windowWidth, _windowHeight);
  glClearColor(0., .5, 1., 1.);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto& program : _programs) {
    program.use();
    for (const auto& vao : program.getVertexArrays()) {
      vao.bind();
      glDrawElements(
        GL_TRIANGLES, vao.getIndexCount(), GL_UNSIGNED_SHORT, nullptr
      );
      vao.unbind();
    }
  }
}

namespace {

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/,
  GLsizei /*length*/, const GLchar* message, const void* /*userParam*/
) -> void {
  LOG_ERROR("GL error: " << message << '\n');
}
#endif // DEBUG

auto initializeGL() -> bool {
  if (!gladLoadGL(glfwGetProcAddress)) {
    return false;
  }
#ifdef DEBUG
  if (GLAD_GL_ARB_debug_output) {
    LOG("GL extension GL_ARB_debug_output available\n");
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(debugMessageCallbackGL, nullptr /*userParam*/);
  }
  else {
    LOG("GL extension GL_ARB_debug_output unavailable\n");
  }
#endif
  LOG("C++ version: " << STRING(__cplusplus) << '\n');
  LOG("Driver OpenGL version: " << glGetString(GL_VERSION) << '\n');
  return true;
}

} // namespace
