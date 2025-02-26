#include "graphics-types.hxx"

#include <optional>
#include <stdexcept>

#include "debug.hxx"
#include "models.hxx"

#ifdef DEBUG
#define LOG_ERROR_INVALID(x) LOG_ERROR("Invalid " << x << '\n')
#define LOG_MOVING(x) LOG("Moving " << x << '\n')
#define LOG_MOVE_ASSIGNING(x) LOG("Move-assigning " << x << '\n')
#define LOG_CLEANING_UP(x) LOG("Cleaning up " << x << '\n')
#else
#define LOG_ERROR_INVALID(x)
#define LOG_MOVING(x)
#define LOG_MOVE_ASSIGNING(x)
#define LOG_CLEANING_UP(x)
#endif // DEBUG

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
  LOG_MOVING(buffer);
  buffer._valid = false;
}

auto my::Buffer::operator=(Buffer&& buffer) -> Buffer& {
  LOG_MOVE_ASSIGNING(buffer);
  _target = buffer._target;
  _id = buffer._id;
  buffer._valid = false;
  return *this;
}

my::Buffer::~Buffer() {
  if (!_valid) {
    return;
  }
  LOG_CLEANING_UP(*this);
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
#ifdef DEBUG
  if (!_valid) {
    LOG_ERROR_INVALID(*this);
    throw std::runtime_error{"Attempt to bind invalid buffer"};
  }
#endif // DEBUG
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

my::VertexArrayBuilder::VertexArrayBuilder(ShaderProgram& program)
: _program{program} {}

auto my::operator<<(VertexArrayBuilder& builder, ShaderAttribute* attribute)
-> VertexArrayBuilder& {
  builder._attributes.push_back(attribute);
  return builder;
}

auto my::operator<<(VertexArrayBuilder& builder, Buffer* buffer)
-> VertexArrayBuilder& {
  builder._indexBuffer = buffer;
  return builder;
}

auto my::VertexArrayBuilder::setIndexCount(GLint indexCount) -> void {
#ifdef DEBUG
  if (indexCount <= 0) {
    throw std::runtime_error{
      "Attempt to set invalid index count on vertex array builder"
    };
  }
#endif // DEBUG
  _indexCount = indexCount;
}

auto my::VertexArrayBuilder::build() -> VertexArray {
#ifdef DEBUG
  if (_indexCount <= 0) {
    throw std::runtime_error{
      "Attempt to build vertex array with invalid/missing index count"
    };
  }
#endif // DEBUG

  GLuint id;
  glGenVertexArrays(1, &id);
  glBindVertexArray(id);

  for (const auto attribute : _attributes) {
    const GLint location{glGetAttribLocation(
      _program.getID(), attribute->name.data()
    )};
    attribute->buffer.bind();
    glVertexAttribPointer(
      location, attribute->size, static_cast<GLenum>(attribute->type),
      attribute->normalized, attribute->stride, attribute->pointer
    );
    glEnableVertexAttribArray(location);
  }

  if (_indexBuffer) {
    _indexBuffer->bind();
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  _attributes.clear();
  _indexBuffer = nullptr;

  return {id, _indexCount};
}

my::VertexArray::VertexArray(GLuint id, GLsizei indexCount)
: _id{id}, _indexCount{indexCount} {}

my::VertexArray::VertexArray(VertexArray&& vao)
: _id{vao._id}, _indexCount{vao._indexCount} {
  LOG_MOVING(vao);
  vao._valid = false;
}

auto my::VertexArray::operator=(VertexArray&& vao) -> VertexArray& {
  LOG_MOVE_ASSIGNING(vao);
  _id = vao._id;
  _indexCount = vao._indexCount;
  vao._valid = false;
  return *this;
}

my::VertexArray::~VertexArray() {
  if (!_valid) {
    return;
  }
  LOG_CLEANING_UP(*this);
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
#ifdef DEBUG
  if (!_valid) {
    LOG_ERROR_INVALID(*this);
    throw std::runtime_error{"Attempt to bind invalid vertex array"};
  }
#endif // DEBUG
  glBindVertexArray(_id);
}

auto my::VertexArray::unbind() const -> void {
  glBindVertexArray(0);
}

auto my::VertexArray::drawTriangles() const -> void {
  glDrawElements(
    GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, nullptr
  );
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
  LOG_MOVING(shader);
  shader._valid = false;
}

auto my::Shader::operator=(Shader&& shader) -> Shader& {
  LOG_MOVE_ASSIGNING(shader);
  _type = shader._type;
  _id = shader._id;
  shader._valid = false;
  return *this;
}

my::Shader::~Shader() {
  if (!_valid) {
    return;
  }
  LOG_CLEANING_UP(*this);
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

my::Uniform::Uniform(const ShaderProgram& program, std::string_view name)
: _location{glGetUniformLocation(program.getID(), name.data())} {
  if (_location < 0) {
    throw std::runtime_error{
      "Attempt to get location for invalid uniform"
    };
  }
}

my::Uniform::Uniform(Uniform&& uniform) : _location{uniform._location} {
  LOG_MOVING(*this);
  uniform._valid = false;
}

auto my::Uniform::operator=(Uniform&& uniform) -> Uniform& {
  LOG_MOVE_ASSIGNING(*this);
  _location = uniform._location;
  uniform._valid = false;
  return *this;
}

my::Uniform::~Uniform() {
  if (!_valid) {
    return;
  }
  LOG_CLEANING_UP(*this);
}

#ifdef DEBUG
auto my::operator<<(std::ostream& out, const Uniform& uniform)
-> std::ostream& {
  out << "Uniform(location=" << uniform._location << ")";
  return out;
}
#endif // DEBUG

auto my::Uniform::getLocation() const -> GLint {
  return _location;
}

template<>
auto my::Uniform::setData(const glm::mat4& data) const -> void {
  glUniformMatrix4fv(_location, 1, false, glm::value_ptr(data));
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
    LOG_ERROR("GL program error: " << log << '\n');
    log.clear();

    glGetShaderiv(vertexShader.getID(), GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(vertexShader.getID(), logLength, nullptr, log.data());
      LOG_ERROR("GL vertex shader error: " << log << '\n');
      log.clear();
    }

    glGetShaderiv(fragmentShader.getID(), GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(
        fragmentShader.getID(), logLength, nullptr, log.data()
      );
      LOG_ERROR("GL fragment shader error: " << log << '\n');
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
: _id{program._id}, _vertexArrays{std::move(program._vertexArrays)},
  _uniforms{std::move(program._uniforms)} {
  LOG_MOVING(program);
  program._valid = false;
}

auto my::ShaderProgram::operator=(ShaderProgram&& program) -> ShaderProgram& {
  LOG_MOVE_ASSIGNING(program);
  _id = program._id;
  _vertexArrays = std::move(program._vertexArrays);
  _uniforms = std::move(program._uniforms);
  program._valid = false;
  return *this;
}

my::ShaderProgram::~ShaderProgram() {
  if (!_valid) {
    return;
  }
  LOG_CLEANING_UP(*this);
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

auto my::ShaderProgram::getVertexArrayBuilder() const
-> const VertexArrayBuilder& {
  return _vertexArrayBuilder;
}

auto my::ShaderProgram::getVertexArrayBuilder() -> VertexArrayBuilder& {
  return _vertexArrayBuilder;
}

auto my::ShaderProgram::getVertexArrays() const
-> const std::vector<VertexArray>&
{
  return _vertexArrays;
}

auto my::ShaderProgram::getVertexArrays() -> std::vector<VertexArray>& {
  return _vertexArrays;
}

auto my::ShaderProgram::getUniforms() const -> const std::vector<Uniform>& {
  return _uniforms;
}

auto my::ShaderProgram::getUniforms() -> std::vector<Uniform>& {
  return _uniforms;
}

auto my::ShaderProgram::use() const -> void {
#ifdef DEBUG
  if (!_valid) {
    LOG_ERROR_INVALID(*this);
    throw std::runtime_error{"Attempt to use invalid shader program"};
  }
#endif // DEBUG
  glUseProgram(_id);
}
