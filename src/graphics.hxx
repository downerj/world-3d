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

  auto getCamera() const -> const Camera&;
  auto getCamera() -> Camera&;
  auto resize(int width, int height) -> void;
  auto render() -> void;

private:
  int _windowWidth{};
  int _windowHeight{};
  std::vector<ShaderProgram> _programs{};
  std::vector<Buffer> _buffers{};
  Camera _camera{glm::radians(90.f), 1.f, 0.1f, 100.f};
  // Camera _camera{};
};

} // namespace my

#endif // GRAPHICS_HXX
