#include "graphics-types.hxx"

#include <optional>
#include <stdexcept>

#include "debug.hxx"
#include "models.hxx"

/*
 * Definitions.
 */

my::Buffer::Buffer(
  BufferTarget target, const GLvoid* data, GLsizei size, BufferUsage usage
) : _target{target} {
  const auto targetGL{static_cast<GLenum>(target)};
  glGenBuffers(1, &_id);
  glBindBuffer(targetGL, _id);
  glBufferData(targetGL, size, data, static_cast<GLenum>(usage));
  glBindBuffer(targetGL, 0);
}

my::Buffer::Buffer(Buffer&& buffer)
: _target{buffer._target}, _id{buffer._id} {
  LOG("Moving " << *this << '\n');
  buffer._valid = false;
}

auto my::Buffer::operator=(Buffer&& buffer) -> Buffer& {
  LOG("Move-assigning " << *this << '\n');
  _target = buffer._target;
  _id = buffer._id;
  buffer._valid = false;
  return *this;
}

my::Buffer::~Buffer() {
  if (!_valid) {
    return;
  }
  LOG("Cleaning up " << *this << '\n');
  glDeleteBuffers(1, &_id);
}

#ifdef DEBUG
auto my::operator<<(std::ostream& out, const my::Buffer& buffer)
-> std::ostream& {
  out << "Buffer(id=" << buffer._id << ", target=";
  switch (buffer._target) {
    case BufferTarget::Array: {
      out << "Array";
      break;
    }
    case BufferTarget::ElementArray: {
      out << "ElementArray";
      break;
    }
    default: {
      out << "?";
      break;
    }
  }
  out << ')';
  return out;
}
#endif // DEBUG

auto my::Buffer::getID() const -> GLuint {
  return _id;
}

auto my::Buffer::bind() const -> void {
  glBindBuffer(static_cast<GLenum>(_target), _id);
}

auto my::Buffer::unbind() const -> void {
  glBindBuffer(static_cast<GLenum>(_target), 0);
}

my::ShaderAttribute::ShaderAttribute(
  std::string_view name_, const Buffer& buffer_, GLint size_,
  AttributeType type_, GLboolean normalized_, GLint stride_,
  const GLvoid* pointer_
) : name{name_}, buffer{buffer_}, size{size_}, type{type_},
    normalized{normalized_}, stride{stride_}, pointer{pointer_} {}

my::VertexArray::VertexArray(VertexArray&& vao)
: _id{vao._id}, _indexCount{vao._indexCount} {
  LOG("Moving " << vao << '\n');
  vao._valid = false;
}

auto my::VertexArray::operator=(VertexArray&& vao) -> VertexArray& {
  LOG("Move-assigning " << vao << '\n');
  _id = vao._id;
  _indexCount = vao._indexCount;
  vao._valid = false;
  return *this;
}

my::VertexArray::~VertexArray() {
  if (!_valid) {
    return;
  }
  LOG("Cleaning up " << *this << '\n');
  glDeleteVertexArrays(1, &_id);
}

#ifdef DEBUG
auto my::operator<<(std::ostream& out, const VertexArray& vao)
-> std::ostream& {
  out << "VertexArray(id=" << vao._id;
  out << ", indexCount=" << vao._indexCount << ')';
  return out;
}
#endif // DEBUG

auto my::VertexArray::getID() const -> GLuint {
  return _id;
}

auto my::VertexArray::getIndexCount() const -> GLsizei {
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
  const GLchar* sources[]{source.data()};
  const GLint lengths[]{static_cast<GLint>(source.size())};
  glShaderSource(_id, 1, sources, lengths);
  glCompileShader(_id);
}

my::Shader::Shader(Shader&& shader) : _type{shader._type}, _id{shader._id} {
  LOG("Moving " << shader << '\n');
  shader._valid = false;
}

auto my::Shader::operator=(Shader&& shader) -> Shader& {
  LOG("Move-assigning " << shader << '\n');
  _type = shader._type;
  _id = shader._id;
  shader._valid = false;
  return *this;
}

my::Shader::~Shader() {
  if (!_valid) {
    return;
  }
  LOG("Cleaning up " << *this << '\n');
  glDeleteShader(_id);
}

#ifdef DEBUG
auto my::operator<<(std::ostream& out, const Shader& shader) -> std::ostream& {
  out << "Shader(id=" << shader._id << ", type=";
  switch (shader._type) {
    case ShaderType::Vertex: {
      out << "Vertex";
      break;
    }
    case ShaderType::Fragment: {
      out << "Fragment";
      break;
    }
    default: {
      out << "?";
      break;
    }
  }
  out << ')';
  return out;
}
#endif // DEBUG

auto my::Shader::getType() const -> ShaderType {
  return _type;
}

auto my::Shader::getID() const -> GLuint {
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
    GLint logLength{};
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

my::ShaderProgram::ShaderProgram(ShaderProgram&& program)
: _id{program._id}, _vertexArrays{std::move(program._vertexArrays)} {
  LOG("Moving " << program << '\n');
  program._valid = false;
}

auto my::ShaderProgram::operator=(ShaderProgram&& program) -> ShaderProgram& {
  LOG("Move-assigning " << program << '\n');
  _id = program._id;
  _vertexArrays = std::move(program._vertexArrays);
  program._valid = false;
  return *this;
}

my::ShaderProgram::~ShaderProgram() {
  if (!_valid) {
    return;
  }
  LOG("Cleaning up " << *this << '\n');
  glDeleteProgram(_id);
}

#ifdef DEBUG
auto my::operator<<(std::ostream& out, const ShaderProgram& program)
-> std::ostream& {
  out << "ShaderProgram(id=" << program._id << ')';
  return out;
}
#endif // DEBUG

auto my::ShaderProgram::getID() const -> GLuint {
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
