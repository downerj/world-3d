#ifndef GRAPHICS_TYPES_HXX
#define GRAPHICS_TYPES_HXX

#include <cstddef>
#ifdef DEBUG
#include <iostream>
#endif // DEBUG
#include <string_view>
#include <vector>

#include <glad/gl.h>

/*
 * Declarations.
 */

namespace my {

class ShaderProgram;

enum class BufferTarget {
  Array = GL_ARRAY_BUFFER,
  ElementArray = GL_ELEMENT_ARRAY_BUFFER,
  /* ... */
};

enum class BufferUsage {
  StaticDraw = GL_STATIC_DRAW,
  /* ... */
};

class Buffer {
public:
  Buffer(
    BufferTarget target, const GLvoid* data, GLsizei size,
    BufferUsage usage = BufferUsage::StaticDraw
  );
  Buffer() = delete;
  Buffer(const Buffer&) = delete;
  Buffer(Buffer&& buffer);
  auto operator=(const Buffer&) -> Buffer& = delete;
  auto operator=(Buffer&& buffer) -> Buffer&;
  ~Buffer() noexcept;
#ifdef DEBUG
  friend auto operator<<(std::ostream&, const Buffer&)
  -> std::ostream&;
#endif // DEBUG
  auto getID() const -> GLuint;
  auto bind() const -> void;
  auto unbind() const -> void;

private:
  BufferTarget _target;
  GLuint _id{};
  bool _valid{true};
};

#ifdef DEBUG
auto operator<<(std::ostream& out, const Buffer& buffer) -> std::ostream&;
#endif // DEBUG

enum class AttributeType {
  Byte = GL_BYTE,
  UnsignedByte = GL_UNSIGNED_BYTE,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Int = GL_INT,
  UnsignedInt = GL_UNSIGNED_INT,
  Float = GL_FLOAT,
  Double = GL_DOUBLE,
  /* ... */
};

struct ShaderAttribute {
  std::string_view name{};
  const Buffer& buffer;
  GLint size{};
  AttributeType type{};
  GLboolean normalized{};
  GLsizei stride{};
  const GLvoid* pointer{};

  ShaderAttribute(
    std::string_view name, const Buffer& buffer, GLint size, AttributeType type,
    GLboolean normalized, GLint stride, const GLvoid* pointer
  );
  ShaderAttribute() = delete;
};

class VertexArray {
public:
  template<typename Container>
  VertexArray(
    const ShaderProgram& program,
    const Container& attributes,
    const Buffer& indexBuffer,
    GLsizei indexCount
  );
  VertexArray() = delete;
  VertexArray(const VertexArray&) = delete;
  VertexArray(VertexArray&& vao);
  auto operator=(const VertexArray&) -> VertexArray& = delete;
  auto operator=(VertexArray&& vao) -> VertexArray&;
  ~VertexArray() noexcept;
#ifdef DEBUG
  friend auto operator<<(std::ostream&, const VertexArray&) -> std::ostream&;
#endif // DEBUG
  auto getID() const -> GLuint;
  auto getIndexCount() const -> GLsizei;
  auto bind() const -> void;
  auto unbind() const -> void;

private:
  GLuint _id{};
  GLsizei _indexCount;
  bool _valid{true};
};

#ifdef DEBUG
auto operator<<(std::ostream& out, const VertexArray& vao) -> std::ostream&;
#endif // DEBUG

enum class ShaderType {
  Vertex = GL_VERTEX_SHADER,
  Fragment = GL_FRAGMENT_SHADER,
  /* ... */
};

class Shader {
public:
  Shader(ShaderType type, std::string_view source);
  Shader() = delete;
  Shader(const Shader&) = delete;
  Shader(Shader&& shader);
  ~Shader() noexcept;
  auto operator=(const Shader&) -> Shader& = delete;
  auto operator=(Shader&& shader) -> Shader&;
#ifdef DEBUG
  friend auto operator<<(std::ostream&, const Shader&) -> std::ostream&;
#endif // DEBUG
  auto getType() const -> ShaderType;
  auto getID() const -> GLuint;

private:
  ShaderType _type;
  GLuint _id;
  bool _valid{true};
};

#ifdef DEBUG
auto operator<<(std::ostream& out, const Shader& shader) -> std::ostream&;
#endif // DEBUG

class ShaderProgram {
public:
  ShaderProgram(const Shader& vertex, const Shader& fragment);
  ShaderProgram() = delete;
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&& program);
  auto operator=(const ShaderProgram&) -> ShaderProgram& = delete;
  auto operator=(ShaderProgram&& program) -> ShaderProgram&;
  ~ShaderProgram() noexcept;
#ifdef DEBUG
  friend auto operator<<(std::ostream&, const ShaderProgram&) -> std::ostream&;
#endif // DEBUG
  auto getID() const -> GLuint;
  auto getVertexArrays() const -> const std::vector<VertexArray>&;
  auto getVertexArrays() -> std::vector<VertexArray>&;
  auto use() const -> void;

private:
  GLuint _id;
  std::vector<VertexArray> _vertexArrays{};
  bool _valid{true};
};

#ifdef DEBUG
auto operator<<(std::ostream& out, const ShaderProgram& program)
-> std::ostream&;
#endif // DEBUG

} // namespace my

/*
 * Definitions.
 */

template<typename Container>
my::VertexArray::VertexArray(
  const ShaderProgram& program,
  const Container& attributes,
  const Buffer& indexBuffer,
  GLsizei indexCount
) : _indexCount{indexCount} {
  glGenVertexArrays(1, &_id);
  glBindVertexArray(_id);

  for (const auto& attribute : attributes) {
    const GLint location{glGetAttribLocation(
      program.getID(), attribute.name.data()
    )};
    attribute.buffer.bind();
    glVertexAttribPointer(
      location, attribute.size, static_cast<GLenum>(attribute.type),
      attribute.normalized, attribute.stride, attribute.pointer
    );
    glEnableVertexAttribArray(location);
  }

  indexBuffer.bind();

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#endif // GRAPHICS_TYPES_HXX
