#include "models.hxx"

#include <array>

/*
 * Declarations.
 */

namespace {

constexpr std::array<GLfloat, 3*3> BasicTriangle_vertices{
  1., -1., 1.,
  -1., -1., 1.,
  0., 1., 1.
};

constexpr std::array<GLfloat, 3*3> BasicTriangle_colors{
  1., 0., 0.,
  0., 1., 0.,
  0., 0., 1.
};

constexpr std::array<GLushort, 3*1> BasicTriangle_indices{
  0, 1, 2
};

} // namespace

/*
 * Definitions.
 */

my::Geometry::~Geometry() {}

auto my::Geometry::getVertexMemorySize() const -> GLsizeiptr {
  return getVertexArraySize()*sizeof(GLfloat);
}

auto my::Geometry::getColorMemorySize() const -> GLsizeiptr {
  return getColorArraySize()*sizeof(GLfloat);
}

auto my::Geometry::getIndexMemorySize() const -> GLsizeiptr {
  return getIndexArraySize()*sizeof(GLushort);
}

auto my::Geometry::getVertexCount() const -> GLint {
  return getVertexArraySize()/3;
}

auto my::Geometry::getColorCount() const -> GLint {
  return getColorArraySize()/3;
}

auto my::Geometry::getIndexCount() const -> GLint {
  return getIndexArraySize();
}

auto my::BasicTriangle::getVertices() const -> const GLfloat* {
  return BasicTriangle_vertices.data();
}

auto my::BasicTriangle::getColors() const -> const GLfloat* {
  return BasicTriangle_colors.data();
}

auto my::BasicTriangle::getIndices() const -> const GLushort* {
  return BasicTriangle_indices.data();
}

auto my::BasicTriangle::getVertexArraySize() const -> GLint {
  return BasicTriangle_vertices.size();
}

auto my::BasicTriangle::getColorArraySize() const -> GLint {
  return BasicTriangle_colors.size();
}

auto my::BasicTriangle::getIndexArraySize() const -> GLint {
  return BasicTriangle_indices.size();
}
