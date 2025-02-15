#include "models.hxx"

#include <array>

namespace {

constexpr std::array<float, 3*3> BasicTriangle_vertices{
  1., -1., 0.,
  -1., -1., 0.,
  0., 1., 0.
};

constexpr std::array<float, 3*3> BasicTriangle_colors{
  1., 0., 0.,
  0., 1., 0.,
  0., 0., 1.
};

constexpr std::array<unsigned short, 3*1> BasicTriangle_indices{
  0, 1, 2
};

} // namespace

Geometry::~Geometry() {}

auto Geometry::getVertexMemorySize() const -> std::size_t {
  return getVertexArraySize()*sizeof(float);
}

auto Geometry::getColorMemorySize() const -> std::size_t {
  return getColorArraySize()*sizeof(float);
}

auto Geometry::getIndexMemorySize() const -> std::size_t {
  return getIndexArraySize()*sizeof(unsigned short);
}

auto Geometry::getVertexCount() const -> int {
  return getVertexArraySize()/3;
}

auto Geometry::getColorCount() const -> int {
  return getColorArraySize()/3;
}

auto Geometry::getIndexCount() const -> int {
  return getIndexArraySize();
}

auto BasicTriangle::getVertices() const -> const float* {
  return BasicTriangle_vertices.data();
}

auto BasicTriangle::getColors() const -> const float* {
  return BasicTriangle_colors.data();
}

auto BasicTriangle::getIndices() const -> const unsigned short* {
  return BasicTriangle_indices.data();
}

auto BasicTriangle::getVertexArraySize() const -> int {
  return BasicTriangle_vertices.size();
}

auto BasicTriangle::getColorArraySize() const -> int {
  return BasicTriangle_colors.size();
}

auto BasicTriangle::getIndexArraySize() const -> int {
  return BasicTriangle_indices.size();
}
