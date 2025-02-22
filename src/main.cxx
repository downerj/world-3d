#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "debug.hxx"
#include "graphics.hxx"
#include "io.hxx"
#include "window.hxx"

auto main(int /*argc*/, char** /*argv*/) -> int {
  try {
    my::WindowHandler window{};
    my::GraphicsEngine graphics{};
    LOG("Begin main loop\n");
    graphics.getCamera().move(2., 2., 2.);
    while (window.isActive()) {
      const my::WindowActions& actions{window.getActions()};
      if (actions.close) {
        window.close();
        break;
      }
      if (actions.resetSize) {
        window.resetSize();
      }
      window.resetActions();
      window.preRender();
      graphics.resize(window.getWidth(), window.getHeight());
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
