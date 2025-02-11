#include "graphics.hxx"

#include <array>
#include <initializer_list>
#include <stdexcept>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "debug.hxx"

/*
 * Declarations.
 */

struct ShaderAttribute {
  std::string name;
  GLuint buffer;
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  GLvoid* pointer;

  ShaderAttribute(
    std::string name, GLuint buffer, GLint size, GLenum type,
    GLboolean normalized, GLsizei stride, GLvoid* pointer
  );
  ShaderAttribute() = delete;
  ShaderAttribute(const ShaderAttribute&) = default;
  ShaderAttribute(ShaderAttribute&&) = default;
  ShaderAttribute& operator=(const ShaderAttribute&) = default;
  ShaderAttribute& operator=(ShaderAttribute&&) = default;
  ~ShaderAttribute() noexcept = default;
};

#ifdef DEBUG
static auto debugMessageCallbackGL(
  GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar* message, const void* userParam
) -> void;
#endif // DEBUG
static auto initializeGL() -> bool;
static auto createShader(GLenum type, std::string_view source) -> GLuint;
static auto createProgram(
  std::string_view vertexSource, std::string_view fragmentSource
) -> std::optional<GLuint>;
template<typename T>
static auto createBuffer(
  GLenum target, const T& data, GLenum usage = GL_STATIC_DRAW
) -> GLuint;
static auto createVertexArray(
  GLuint program,
  std::initializer_list<ShaderAttribute> attributes,
  GLuint indexBuffer
) -> GLuint;
static auto createShaderData(
  std::string_view vertexSource, std::string_view fragmentSource
) -> std::optional<ShaderData>;

/*
 * Definitions.
 */

ShaderAttribute::ShaderAttribute(
  std::string name_, GLuint buffer_, GLint size_, GLenum type_,
  GLboolean normalized_, GLsizei stride_, GLvoid* pointer_
) : name{name_}, buffer{buffer_}, size{size_}, type{type_},
    normalized{normalized_}, stride{stride_}, pointer{pointer_} {}

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/,
  GLsizei /*length*/, const GLchar* message, const void* /*userParam*/
) -> void {
  LOG_ERROR("GL error: " << message << '\n');
}
#endif // DEBUG

ShaderData::ShaderData(
  GLuint program_, GLuint vao_, GLsizei indexCount_
) : program{program_}, vao{vao_}, indexCount{indexCount_} {}

GraphicsEngine::GraphicsEngine(
  std::string_view vertexSource, std::string_view fragmentSource
) {
  if (!initializeGL()) {
    throw std::runtime_error{"Failed to initialize OpenGL"};
  }
  std::optional<ShaderData> shaderData{createShaderData(
    vertexSource, fragmentSource
  )};
  if (!shaderData) {
    throw std::runtime_error{"Failed to create shader program"};
  }
  _shaderDatas.push_back(*shaderData);
}

GraphicsEngine::~GraphicsEngine() {
  for (const auto& shaderData : _shaderDatas) {
    glDeleteVertexArrays(1, &shaderData.vao);
    glDeleteProgram(shaderData.program);
  }
}

auto GraphicsEngine::resize(int width, int height) -> void {
  _windowWidth = width;
  _windowHeight = height;
}

auto GraphicsEngine::render() -> void {
  glViewport(0, 0, _windowWidth, _windowHeight);
  glClearColor(0., .5, 1., 1.);
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto& shaderData : _shaderDatas) {
    glUseProgram(shaderData.program);
    glBindVertexArray(shaderData.vao);
    glDrawElements(
      GL_TRIANGLES, shaderData.indexCount, GL_UNSIGNED_SHORT, nullptr
    );
    glBindVertexArray(0);
  }
}

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

auto createShader(GLenum type, std::string_view source) -> GLuint {
  GLuint shader{glCreateShader(type)};
  const GLchar* sources[]{source.data()};
  const GLint lengths[]{static_cast<GLint>(source.size())};
  glShaderSource(shader, 1, sources, lengths);
  glCompileShader(shader);
  return shader;
}

auto createProgram(
  std::string_view vertexSource, std::string_view fragmentSource
) -> std::optional<GLuint> {
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
    GLsizei logLength{};
    std::string log{};

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    log.resize(logLength);
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    std::cerr << "GL program error: " << log << '\n';
    log.clear();

    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(vertexShader, logLength, nullptr, log.data());
      std::cerr << "GL vertex shader error: " << log << '\n';
      log.clear();
    }

    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());
      std::cerr << "GL fragment shader error: " << log << '\n';
      log.clear();
    }
  }
#endif // DEBUG
  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  if (!status) {
    return {};
  }
  return program;
}

template<typename T>
auto createBuffer(GLenum target, const T& data, GLenum usage) -> GLuint {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(
    target, sizeof(typename T::value_type)*data.size(), data.data(), usage
  );
  glBindBuffer(target, 0);
  return buffer;
}

auto createVertexArray(
  GLuint program,
  std::initializer_list<ShaderAttribute> attributes,
  GLuint indexBuffer
) -> GLuint {
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  for (const auto& attribute : attributes) {
    const GLint location{glGetAttribLocation(program, attribute.name.c_str())};
    glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer);
    glVertexAttribPointer(
      location, attribute.size, attribute.type,
      attribute.normalized, attribute.stride, attribute.pointer
    );
    glEnableVertexAttribArray(location);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return vao;
}

auto createShaderData(
  std::string_view vertexSource, std::string_view fragmentSource
) -> std::optional<ShaderData> {
  const std::optional<GLuint> program{
    createProgram(vertexSource, fragmentSource)
  };
  if (!program) {
    return {};
  }

  const std::array<GLfloat, 3*3> vertices{
    1., -1., 0.,
    -1., -1., 0.,
    0., 1., 0.,
  };
  const std::array<GLfloat, 3*3> colors{
    1., 0., 0.,
    0., 1., 0.,
    0., 0., 1.
  };
  const std::array<GLushort, 3*1> indices{
    0, 1, 2
  };
  const ShaderAttribute positionAttribute{
    "position", createBuffer(GL_ARRAY_BUFFER, vertices),
    3, GL_FLOAT, false, 0, nullptr
  };
  const ShaderAttribute colorAttribute{
    "color", createBuffer(GL_ARRAY_BUFFER, colors),
    3, GL_FLOAT, false, 0, nullptr
  };
  const GLuint indexBuffer{createBuffer(GL_ELEMENT_ARRAY_BUFFER, indices)};
  const GLuint vao{createVertexArray(
    *program, {positionAttribute, colorAttribute}, indexBuffer
  )};

  return {{*program, vao, 3}};
}
