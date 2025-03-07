#include "graphics-engine.hxx"

#include <array>
#include <cmath>
#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
auto buildProgram(
  std::string_view vertexPath, std::string_view fragmentPath
) -> my::ShaderProgram;
constexpr const char* mainVertexPath{"res/shaders/main.vert"};
constexpr const char* mainFragmentPath{"res/shaders/main.frag"};

} // namespace

/*
 * Definitions.
 */

my::GraphicsEngine::GraphicsEngine()
: _glAvailable{initializeGL()},
  _mainProgram{buildProgram(mainVertexPath, mainFragmentPath)} {
  if (!_glAvailable) {
    throw std::runtime_error{"Failed to initialize OpenGL"};
  }

  BasicTriangle triangle{};
  Geometry& geometry{triangle};
  Buffer positionBuffer{
    BufferTarget::Array, geometry.getVertices(),
    static_cast<GLsizei>(geometry.getVertexMemorySize())
  };
  Buffer colorBuffer{
    BufferTarget::Array, geometry.getColors(),
    static_cast<GLsizei>(geometry.getColorMemorySize())
  };
  Buffer indexBuffer{
    BufferTarget::ElementArray, geometry.getIndices(),
    static_cast<GLsizei>(geometry.getIndexMemorySize())
  };
  ShaderAttribute positionAttribute{
    "position", positionBuffer, geometry.getVertexCount(),
    AttributeType::Float, false, 0, nullptr
  };
  ShaderAttribute colorAttribute{
    "color", colorBuffer, geometry.getColorCount(),
    AttributeType::Float, false, 0, nullptr
  };
  VertexArrayBuilder& vaoBuilder{_mainProgram.getVertexArrayBuilder()};
  vaoBuilder.setIndexCount(geometry.getIndexCount());
  vaoBuilder << &indexBuffer;
  vaoBuilder << &positionAttribute << &colorAttribute;
  VertexArray vao{vaoBuilder.build()};
  std::vector<VertexArray>& vertexArrays{_mainProgram.getVertexArrays()};
  vertexArrays.reserve(1);
  vertexArrays.push_back(std::move(vao));
  std::vector<Uniform>& uniforms{_mainProgram.getUniforms()};
  uniforms.reserve(3);
  uniforms.push_back({_mainProgram, "projection"});
  uniforms.push_back({_mainProgram, "view"});
  uniforms.push_back({_mainProgram, "model"});
  _buffers.push_back(std::move(positionBuffer));
  _buffers.push_back(std::move(colorBuffer));
  _buffers.push_back(std::move(indexBuffer));
}

auto my::GraphicsEngine::setCamera(const Camera* camera) -> void {
  _camera = camera;
}

auto my::GraphicsEngine::resize(int width, int height) -> void {
  _windowWidth = width;
  _windowHeight = height;
}

auto my::GraphicsEngine::render() const -> void {
  if (!_camera) {
    return;
  }
  resetFrame();
  _mainProgram.use();
  const Uniform& projectionUniform{_mainProgram.getUniforms().at(0)};
  const Uniform& viewUniform{_mainProgram.getUniforms().at(1)};
  const Uniform& modelUniform{_mainProgram.getUniforms().at(2)};
  projectionUniform.setData(_camera->getProjectionMatrix());
  viewUniform.setData(_camera->getViewMatrix());
  glm::mat4 modelMatrix{1.};
  for (const auto& vao : _mainProgram.getVertexArrays()) {
    vao.bind();
    modelUniform.setData(modelMatrix);
    vao.drawTriangles();
    vao.unbind();
  }
}

auto my::GraphicsEngine::resetFrame() const -> void {
  glViewport(0, 0, _windowWidth, _windowHeight);
  glClearColor(0., .5, 1., 1.);
  glClear(GL_COLOR_BUFFER_BIT);
}

namespace {

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* message, const GLvoid*
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

auto buildProgram(
  std::string_view vertexPath, std::string_view fragmentPath
) -> my::ShaderProgram {
  std::optional<std::string> vertexSource{my::readFile(vertexPath)};
  std::optional<std::string> fragmentSource{my::readFile(fragmentPath)};
  if (!vertexSource || !fragmentSource) {
    throw std::runtime_error{"Failed to load shader sources"};
  }
  my::Shader vertexShader{my::ShaderType::Vertex, *vertexSource};
  my::Shader fragmentShader{my::ShaderType::Fragment, *fragmentSource};
  return {vertexShader, fragmentShader};
}

} // namespace
