#include "graphics-types.hxx"

#include <iostream>
#include <optional>
#include <stdexcept>

#include "models.hxx"

/*
 * Definitions.
 */

my::Buffer::Buffer(
  BufferTarget target, const void* data, std::size_t size, BufferUsage usage
) : _target{target} {
  const auto targetGL{static_cast<GLenum>(target)};
  glGenBuffers(1, &_id);
  glBindBuffer(targetGL, _id);
  glBufferData(targetGL, size, data, static_cast<GLenum>(usage));
  glBindBuffer(targetGL, 0);
}

auto my::Buffer::cleanup() -> void {
  glDeleteBuffers(1, &_id);
}

auto my::Buffer::getID() const -> unsigned int {
  return _id;
}

auto my::Buffer::bind() const -> void {
  glBindBuffer(static_cast<GLenum>(_target), _id);
}

auto my::Buffer::unbind() const -> void {
  glBindBuffer(static_cast<GLenum>(_target), 0);
}

my::ShaderAttribute::ShaderAttribute(
  std::string_view name_, const Buffer& buffer_, int size_, AttributeType type_,
  bool normalized_, int stride_, void* pointer_
) : name{name_}, buffer{buffer_}, size{size_}, type{type_},
    normalized{normalized_}, stride{stride_}, pointer{pointer_} {}

auto my::VertexArray::cleanup() -> void {
  glDeleteVertexArrays(1, &_id);
}

auto my::VertexArray::getID() const -> unsigned int {
  return _id;
}

auto my::VertexArray::getIndexCount() const -> int {
  return _indexCount;
}

auto my::VertexArray::bind() const -> void {
  glBindVertexArray(_id);
}

auto my::VertexArray::unbind() const -> void {
  glBindVertexArray(0);
}

my::Shader::Shader(
  ShaderType type, std::string_view source
) : _type{type}, _id{glCreateShader(static_cast<GLenum>(type))} {
  const char* sources[]{source.data()};
  const int lengths[]{static_cast<int>(source.size())};
  glShaderSource(_id, 1, sources, lengths);
  glCompileShader(_id);
}

auto my::Shader::cleanup() const -> void {
  glDeleteShader(_id);
}

auto my::Shader::getType() const -> ShaderType {
  return _type;
}

auto my::Shader::getID() const -> unsigned int {
  return _id;
}

my::ShaderProgram::ShaderProgram(
  const Shader& vertexShader, const Shader& fragmentShader
) : _id{glCreateProgram()} {
  glAttachShader(_id, vertexShader.getID());
  glAttachShader(_id, fragmentShader.getID());
  glLinkProgram(_id);
  GLint status{};
  glGetProgramiv(_id, GL_LINK_STATUS, &status);
#ifdef DEBUG
  if (!status) {
    int logLength{};
    std::string log{};

    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength);
    log.resize(logLength);
    glGetProgramInfoLog(_id, logLength, nullptr, log.data());
    std::cerr << "GL program error: " << log << '\n';
    log.clear();

    glGetShaderiv(vertexShader.getID(), GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(vertexShader.getID(), logLength, nullptr, log.data());
      std::cerr << "GL vertex shader error: " << log << '\n';
      log.clear();
    }

    glGetShaderiv(fragmentShader.getID(), GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(
        fragmentShader.getID(), logLength, nullptr, log.data()
      );
      std::cerr << "GL fragment shader error: " << log << '\n';
      log.clear();
    }
  }
#endif // DEBUG
  glDetachShader(_id, vertexShader.getID());
  glDetachShader(_id, fragmentShader.getID());
  if (!status) {
    glDeleteProgram(_id);
    throw std::runtime_error{"Unable to create program"};
  }
}

auto my::ShaderProgram::cleanup() -> void {
  glDeleteProgram(_id);
  for (auto& vertexArray : _vertexArrays) {
    vertexArray.cleanup();
  }
}

auto my::ShaderProgram::getID() const -> unsigned int {
  return _id;
}

auto my::ShaderProgram::getVertexArrays() const
-> const std::vector<VertexArray>&
{
  return _vertexArrays;
}

auto my::ShaderProgram::getVertexArrays() -> std::vector<VertexArray>& {
  return _vertexArrays;
}

auto my::ShaderProgram::use() const -> void {
  glUseProgram(_id);
}
