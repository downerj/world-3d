#ifndef GAME_HXX
#define GAME_HXX

#include <glm/trigonometric.hpp>

#include "camera.hxx"

/*
 * Declarations.
 */

namespace my {

class Game {
public:
  Game();
  Game(const Game&) = delete;
  Game(Game&&) = delete;
  Game& operator=(const Game&) = delete;
  Game& operator=(Game&&) = delete;

  auto getCamera() const -> const Camera&;
  auto getCamera() -> Camera&;
  auto tick() -> void;

private:
  Camera _camera{glm::radians(90.f), 1.f, 0.1f, 100.f};
};

} // namespace my

#endif // GAME_HXX
