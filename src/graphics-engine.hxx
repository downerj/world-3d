#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string_view>
#include <vector>

#include <glad/gl.h>

#include "camera.hxx"
#include "graphics-types.hxx"

/*
 * Declarations.
 */

namespace my {

class GraphicsEngine {
public:
  GraphicsEngine();
  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine(GraphicsEngine&&) = delete;
  GraphicsEngine& operator=(const GraphicsEngine&) = delete;
  GraphicsEngine& operator=(GraphicsEngine&&) = delete;

  auto setCamera(const Camera* camera) -> void;
  auto resize(int width, int height) -> void;
  auto render() const -> void;

private:
  bool _glAvailable;
  int _windowWidth{};
  int _windowHeight{};
  ShaderProgram _mainProgram;
  std::vector<Buffer> _buffers{};
  const Camera* _camera{nullptr};

  auto resetFrame() const -> void;
};

} // namespace my

#endif // GRAPHICS_HXX
