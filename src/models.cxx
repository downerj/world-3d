#include "models.hxx"

#include <array>

Geometry::~Geometry() {}

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

auto BasicTriangle::getVertices() const -> const float* {
  return BasicTriangle_vertices.data();
}

auto BasicTriangle::getColors() const -> const float* {
  return BasicTriangle_colors.data();
}

auto BasicTriangle::getIndices() const -> const unsigned short* {
  return BasicTriangle_indices.data();
}

auto BasicTriangle::getVertexCount() const -> int {
  return BasicTriangle_vertices.size();
}

auto BasicTriangle::getColorCount() const -> int {
  return BasicTriangle_colors.size();
}

auto BasicTriangle::getIndexCount() const -> int {
  return BasicTriangle_indices.size();
}
