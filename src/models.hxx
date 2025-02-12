#ifndef MODELS_HXX
#define MODELS_HXX

class Geometry {
public:
  virtual auto getVertices() const -> const float* = 0;
  virtual auto getColors() const -> const float* = 0;
  virtual auto getIndices() const -> const unsigned short* = 0;
  virtual auto getVertexCount() const -> int = 0;
  virtual auto getColorCount() const -> int = 0;
  virtual auto getIndexCount() const -> int = 0;
  virtual ~Geometry() noexcept = 0;
};

class BasicTriangle : public Geometry {
public:
  auto getVertices() const -> const float* final;
  auto getColors() const -> const float* final;
  auto getIndices() const -> const unsigned short* final;
  auto getVertexCount() const -> int final;
  auto getColorCount() const -> int final;
  auto getIndexCount() const -> int final;
};

#endif // MODELS_HXX
