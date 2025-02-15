#include "graphics-types.hxx"

#include <iostream>
#include <optional>
#include <stdexcept>

#include "models.hxx"

Buffer::Buffer(
  BufferTarget target, const void* data, std::size_t size, BufferUsage usage
) {
  const auto targetGL{static_cast<GLenum>(target)};
  glGenBuffers(1, &_id);
  glBindBuffer(targetGL, _id);
  glBufferData(targetGL, size, data, static_cast<GLenum>(usage));
  glBindBuffer(targetGL, 0);
}

auto Buffer::getID() const -> unsigned int {
  return _id;
}

auto Buffer::cleanup() -> void {
  glDeleteBuffers(1, &_id);
}

ShaderAttribute::ShaderAttribute(
  std::string name_, const Buffer& buffer_, int size_, AttributeType type_,
  bool normalized_, int stride_, void* pointer_
) : name{name_}, buffer{buffer_}, size{size_}, type{type_},
    normalized{normalized_}, stride{stride_}, pointer{pointer_} {}

VertexArray::VertexArray(
  const ShaderProgram& program,
  std::initializer_list<ShaderAttribute> attributes,
  const Buffer& indexBuffer,
  int indexCount
) : _indexCount{indexCount} {
  glGenVertexArrays(1, &_id);
  glBindVertexArray(_id);

  for (const auto& attribute : attributes) {
    const GLint location{glGetAttribLocation(
      program.getID(), attribute.name.c_str()
    )};
    glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer.getID());
    glVertexAttribPointer(
      location, attribute.size, static_cast<GLenum>(attribute.type),
      attribute.normalized, attribute.stride, attribute.pointer
    );
    glEnableVertexAttribArray(location);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.getID());

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

auto VertexArray::cleanup() -> void {
  glDeleteVertexArrays(1, &_id);
}

auto VertexArray::getID() const -> unsigned int {
  return _id;
}

auto VertexArray::getIndexCount() const -> int {
  return _indexCount;
}

auto VertexArray::bind() const -> void {
  glBindVertexArray(_id);
}

auto VertexArray::unbind() const -> void {
  glBindVertexArray(0);
}

Shader::Shader(
  ShaderType type, std::string_view source
) : _type{type}, _id{glCreateShader(static_cast<GLenum>(type))} {
  const char* sources[]{source.data()};
  const int lengths[]{static_cast<int>(source.size())};
  glShaderSource(_id, 1, sources, lengths);
  glCompileShader(_id);
}

auto Shader::cleanup() const -> void {
  glDeleteShader(_id);
}

auto Shader::getType() const -> ShaderType {
  return _type;
}

auto Shader::getID() const -> unsigned int {
  return _id;
}

ShaderProgram::ShaderProgram(
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

auto ShaderProgram::cleanup() -> void {
  glDeleteProgram(_id);
  for (auto& vertexArray : _vertexArrays) {
    vertexArray.cleanup();
  }
}

auto ShaderProgram::getID() const -> unsigned int {
  return _id;
}

auto ShaderProgram::addVertexArray(VertexArray vertexArray) -> void {
  _vertexArrays.push_back(vertexArray);
}

auto ShaderProgram::getVertexArrays() const -> const std::vector<VertexArray>&
{
  return _vertexArrays;
}

auto ShaderProgram::use() const -> void {
  glUseProgram(_id);
}

////////////////////////////////////

// auto createShader(GLenum type, std::string_view source) -> GLuint;
// auto createProgram(
//   std::string_view vertexSource, std::string_view fragmentSource
// ) -> std::optional<GLuint>;
// template<typename T>
// auto createBuffer(
//   GLenum target, const T* data, GLsizei size, GLenum usage = GL_STATIC_DRAW
// ) -> GLuint;
// auto createVertexArray(
//   GLuint program,
//   std::initializer_list<ShaderAttribute> attributes,
//   GLuint indexBuffer
// ) -> GLuint;
// auto createShaderData(
//   std::string_view vertexSource, std::string_view fragmentSource,
//   const Geometry& geometry
// ) -> std::optional<ShaderProgram>;

// auto createShader(GLenum type, std::string_view source) -> GLuint {
//   GLuint shader{glCreateShader(type)};
//   const GLchar* sources[]{source.data()};
//   const GLint lengths[]{static_cast<GLint>(source.size())};
//   glShaderSource(shader, 1, sources, lengths);
//   glCompileShader(shader);
//   return shader;
// }

// auto createProgram(
//   std::string_view vertexSource, std::string_view fragmentSource
// ) -> std::optional<GLuint> {
//   GLuint vertexShader{createShader(GL_VERTEX_SHADER, vertexSource)};
//   GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentSource)};
//   GLuint program{glCreateProgram()};
//   glAttachShader(program, vertexShader);
//   glAttachShader(program, fragmentShader);
//   glLinkProgram(program);
//   GLint status{};
//   glGetProgramiv(program, GL_LINK_STATUS, &status);
// #ifdef DEBUG
//   if (!status) {
//     GLsizei logLength{};
//     std::string log{};

//     glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
//     log.resize(logLength);
//     glGetProgramInfoLog(program, logLength, nullptr, log.data());
//     std::cerr << "GL program error: " << log << '\n';
//     log.clear();

//     glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
//     if (logLength > 0) {
//       log.resize(logLength);
//       glGetShaderInfoLog(vertexShader, logLength, nullptr, log.data());
//       std::cerr << "GL vertex shader error: " << log << '\n';
//       log.clear();
//     }

//     glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
//     if (logLength > 0) {
//       log.resize(logLength);
//       glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());
//       std::cerr << "GL fragment shader error: " << log << '\n';
//       log.clear();
//     }
//   }
// #endif // DEBUG
//   glDetachShader(program, vertexShader);
//   glDetachShader(program, fragmentShader);
//   glDeleteShader(vertexShader);
//   glDeleteShader(fragmentShader);
//   if (!status) {
//     return {};
//   }
//   return program;
// }

// template<typename T>
// auto createBuffer(
//   GLenum target, const T* data, GLsizei size, GLenum usage
// ) -> GLuint {
//   GLuint buffer;
//   glGenBuffers(1, &buffer);
//   glBindBuffer(target, buffer);
//   glBufferData(target, size, data, usage);
//   glBindBuffer(target, 0);
//   return buffer;
// }

// auto createVertexArray(
//   GLuint program,
//   std::initializer_list<ShaderAttribute> attributes,
//   GLuint indexBuffer
// ) -> GLuint {
//   GLuint vao{};
//   glGenVertexArrays(1, &vao);
//   glBindVertexArray(vao);

//   for (const auto& attribute : attributes) {
//     const GLint location{glGetAttribLocation(program, attribute.name.c_str())};
//     glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer);
//     glVertexAttribPointer(
//       location, attribute.size, attribute.type,
//       attribute.normalized, attribute.stride, attribute.pointer
//     );
//     glEnableVertexAttribArray(location);
//   }

//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

//   glBindVertexArray(0);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//   return vao;
// }

// auto createShaderData(
//   std::string_view vertexSource, std::string_view fragmentSource,
//   const Geometry& geometry
// ) -> std::optional<ShaderProgram> {
//   const std::optional<GLuint> program{
//     createProgram(vertexSource, fragmentSource)
//   };
//   if (!program) {
//     return {};
//   }

//   const GLuint positionBuffer{createBuffer(
//     GL_ARRAY_BUFFER, geometry.getVertices(),
//     sizeof(float)*geometry.getVertexCount()
//   )};
//   const ShaderAttribute positionAttribute{
//     "position", positionBuffer, geometry.getVertexCount()/3,
//     GL_FLOAT, false, 0, nullptr
//   };
//   const GLuint colorBuffer{createBuffer(
//     GL_ARRAY_BUFFER, geometry.getColors(),
//     sizeof(float)*geometry.getColorCount()
//   )};
//   const ShaderAttribute colorAttribute{
//     "color", colorBuffer, geometry.getColorCount()/3,
//     GL_FLOAT, false, 0, nullptr
//   };
//   const GLuint indexBuffer{createBuffer(
//     GL_ELEMENT_ARRAY_BUFFER, geometry.getIndices(),
//     sizeof(unsigned short)*geometry.getIndexCount()
//   )};
//   const GLuint vao{createVertexArray(
//     *program, {positionAttribute, colorAttribute}, indexBuffer
//   )};

//   return {{*program, vao, geometry.getIndexCount()}};
// }
