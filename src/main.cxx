#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "debug.hxx"
#include "game.hxx"
#include "graphics.hxx"
#include "io.hxx"
#include "window.hxx"

auto main(int, char**) -> int {
  try {
    my::Game game{};
    my::WindowHandler window{};
    my::GraphicsEngine graphics{};
    graphics.setCamera(&game.getCamera());
    const my::WindowActions& actions{window.getActions()};
    LOG("Begin main loop\n");
    while (window.isActive()) {
      if (actions.close) {
        window.close();
        break;
      }
      if (actions.resetSize) {
        window.resetSize();
      }
      if (actions.resize) {
        const int width{window.getWidth()};
        const int height{window.getHeight()};
        graphics.resize(width, height);
        game.getCamera().setAspectRatio(width, height);
      }
      window.resetActions();
      window.preRender();
      game.tick();
      game.getCamera().update();
      graphics.render();
      window.postRender();
    }
    LOG("End main loop\n");
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  LOG("Goodbye.\n");
}
