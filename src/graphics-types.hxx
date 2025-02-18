#ifndef GRAPHICS_TYPES_HXX
#define GRAPHICS_TYPES_HXX

#include <cstddef>
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
    BufferTarget target, const void* data, std::size_t size,
    BufferUsage usage = BufferUsage::StaticDraw
  );
  Buffer() = delete;
  auto cleanup() -> void;
  auto getID() const -> unsigned int;
  auto bind() const -> void;
  auto unbind() const -> void;

private:
  BufferTarget _target;
  unsigned int _id{};
};

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
  int size{};
  AttributeType type{};
  bool normalized{};
  int stride{};
  void* pointer{};

  ShaderAttribute(
    std::string_view name, const Buffer& buffer, int size, AttributeType type,
    bool normalized, int stride, void* pointer
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
    int indexCount
  );
  VertexArray() = delete;
  auto cleanup() -> void;
  auto getID() const -> unsigned int;
  auto getIndexCount() const -> int;
  auto bind() const -> void;
  auto unbind() const -> void;

private:
  unsigned int _id{};
  int _indexCount;
};

enum class ShaderType {
  Vertex = GL_VERTEX_SHADER,
  Fragment = GL_FRAGMENT_SHADER,
  /* ... */
};

class Shader {
public:
  Shader(ShaderType type, std::string_view source);
  Shader() = delete;
  auto cleanup() const -> void;
  auto getType() const -> ShaderType;
  auto getID() const -> unsigned int;

private:
  ShaderType _type;
  unsigned int _id;
};

class ShaderProgram {
public:
  ShaderProgram(const Shader& vertex, const Shader& fragment);
  ShaderProgram() = delete;
  auto cleanup() -> void;
  auto getID() const -> unsigned int;
  template<typename... Args>
  auto emplaceVertexArray(Args&&... args) -> void;
  auto getVertexArrays() const -> const std::vector<VertexArray>&;
  auto getVertexArrays() -> std::vector<VertexArray>&;
  auto use() const -> void;

private:
  unsigned int _id;
  std::vector<VertexArray> _vertexArrays{};
};

} // namespace my

/*
 * Definitions.
 */

template<typename Container>
my::VertexArray::VertexArray(
  const ShaderProgram& program,
  const Container& attributes,
  const Buffer& indexBuffer,
  int indexCount
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

template<typename... Args>
auto my::ShaderProgram::emplaceVertexArray(Args&&... args) -> void {
  _vertexArrays.emplace_back(args...);
}

#endif // GRAPHICS_TYPES_HXX
