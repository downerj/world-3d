#ifndef MODELS_HXX
#define MODELS_HXX

#include <cstddef>

#include <glad/gl.h>

/*
 * Declarations.
 */

namespace my {

class Geometry {
public:
  virtual auto getVertices() const -> const GLfloat* = 0;
  virtual auto getColors() const -> const GLfloat* = 0;
  virtual auto getIndices() const -> const GLushort* = 0;
  virtual auto getVertexArraySize() const -> GLint = 0;
  virtual auto getColorArraySize() const -> GLint = 0;
  virtual auto getIndexArraySize() const -> GLint = 0;
  auto getVertexMemorySize() const -> GLsizeiptr;
  auto getColorMemorySize() const -> GLsizeiptr;
  auto getIndexMemorySize() const -> GLsizeiptr;
  auto getVertexCount() const -> GLint;
  auto getColorCount() const -> GLint;
  auto getIndexCount() const -> GLint;
  virtual ~Geometry() noexcept;
};

class BasicTriangle : public Geometry {
public:
  auto getVertices() const -> const GLfloat* final;
  auto getColors() const -> const GLfloat* final;
  auto getIndices() const -> const GLushort* final;
  auto getVertexArraySize() const -> GLint final;
  auto getColorArraySize() const -> GLint final;
  auto getIndexArraySize() const -> GLint final;
};

} // namespace my

#endif // MODELS_HXX
