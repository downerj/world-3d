#include "game.hxx"

/*
 * Definitions.
 */

my::Game::Game() {
  _camera.setPosition(2., 2., 2.);
}

auto my::Game::getCamera() const -> const Camera& {
  return _camera;
}

auto my::Game::getCamera() -> Camera& {
  return _camera;
}

auto my::Game::tick() -> void {}
