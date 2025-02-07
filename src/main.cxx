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
    WindowHandler window{};
    std::optional<std::string> vertexSource{readFile(
      "res/shaders/main.vert"
    )};
    std::optional<std::string> fragmentSource{readFile(
      "res/shaders/main.frag"
    )};
    if (!vertexSource || !fragmentSource) {
      throw std::runtime_error{"Failed to load shader sources"};
    }
    GraphicsEngine graphics{*vertexSource, *fragmentSource};
    while (window.isActive()) {
      const WindowActions& actions{window.getActions()};
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
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}
