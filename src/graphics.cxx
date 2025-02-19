#include "graphics.hxx"

#include <array>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

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
  GLsizei length, const GLchar* message, const GLvoid* userParam
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
  ShaderProgram program{vertexShader, fragmentShader};
  Buffer positionBuffer{
    BufferTarget::Array, geometry->getVertices(),
    static_cast<GLsizei>(geometry->getVertexMemorySize())
  };
  Buffer colorBuffer{
    BufferTarget::Array, geometry->getColors(),
    static_cast<GLsizei>(geometry->getColorMemorySize())
  };
  Buffer indexBuffer{
    BufferTarget::ElementArray, geometry->getIndices(),
    static_cast<GLsizei>(geometry->getIndexMemorySize())
  };
  ShaderAttribute positionAttribute{
    "position", positionBuffer, geometry->getVertexCount(),
    AttributeType::Float, false, 0, nullptr
  };
  ShaderAttribute colorAttribute{
    "color", colorBuffer, geometry->getColorCount(),
    AttributeType::Float, false, 0, nullptr
  };
  VertexArrayBuilder vaoBuilder{};
  vaoBuilder.setIndexCount(geometry->getIndexCount());
  vaoBuilder << &program << &indexBuffer;
  vaoBuilder << &positionAttribute << &colorAttribute;
  VertexArray vao{vaoBuilder.build()};
  program.getVertexArrays().reserve(1);
  program.getVertexArrays().push_back(std::move(vao));
  _programs.reserve(1);
  _programs.push_back(std::move(program));
  _buffers.reserve(3);
  _buffers.push_back(std::move(positionBuffer));
  _buffers.push_back(std::move(colorBuffer));
  _buffers.push_back(std::move(indexBuffer));
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
  GLsizei /*length*/, const GLchar* message, const GLvoid* /*userParam*/
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
