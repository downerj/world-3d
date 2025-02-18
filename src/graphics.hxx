#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string_view>
#include <vector>

#include <glad/gl.h>

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

  auto resize(int width, int height) -> void;
  auto render() -> void;

private:
  int _windowWidth{};
  int _windowHeight{};
  std::vector<ShaderProgram> _programs{};
  std::vector<Buffer> _buffers{};
};

} // namespace my

#endif // GRAPHICS_HXX
