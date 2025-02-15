#ifndef MODELS_HXX
#define MODELS_HXX

#include <cstddef>

class Geometry {
public:
  virtual auto getVertices() const -> const float* = 0;
  virtual auto getColors() const -> const float* = 0;
  virtual auto getIndices() const -> const unsigned short* = 0;
  virtual auto getVertexArraySize() const -> int = 0;
  virtual auto getColorArraySize() const -> int = 0;
  virtual auto getIndexArraySize() const -> int = 0;
  auto getVertexMemorySize() const -> std::size_t;
  auto getColorMemorySize() const -> std::size_t;
  auto getIndexMemorySize() const -> std::size_t;
  auto getVertexCount() const -> int;
  auto getColorCount() const -> int;
  auto getIndexCount() const -> int;
  virtual ~Geometry() noexcept;
};

class BasicTriangle : public Geometry {
public:
  auto getVertices() const -> const float* final;
  auto getColors() const -> const float* final;
  auto getIndices() const -> const unsigned short* final;
  auto getVertexArraySize() const -> int final;
  auto getColorArraySize() const -> int final;
  auto getIndexArraySize() const -> int final;
};

#endif // MODELS_HXX
