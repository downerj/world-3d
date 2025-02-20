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

constexpr auto degreesToRadians(float degrees) -> float;

class GraphicsEngine {
public:
  GraphicsEngine();
  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine(GraphicsEngine&&) = delete;
  GraphicsEngine& operator=(const GraphicsEngine&) = delete;
  GraphicsEngine& operator=(GraphicsEngine&&) = delete;

  auto resize(int width, int height) -> void;
  auto render() -> void;

private:
  int _windowWidth{};
  int _windowHeight{};
  std::vector<ShaderProgram> _programs{};
  std::vector<Buffer> _buffers{};
  // Camera _camera{static_cast<float>(M_PI), 1.f, 0.1f, 100.f};
  Camera _camera{};
};

} // namespace my

/*
 * Definitions.
 */

constexpr auto my::degreesToRadians(float degrees) -> float {
  return static_cast<float>(M_PI) * degrees / 180.f;
}

#endif // GRAPHICS_HXX
