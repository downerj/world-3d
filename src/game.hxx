#ifndef GAME_HXX
#define GAME_HXX

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

  auto tick() -> void;
};

} // namespace my

#endif // GAME_HXX
