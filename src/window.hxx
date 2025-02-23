#ifndef WINDOW_HXX
#define WINDOW_HXX

#include <tuple>

struct GLFWwindow;

/*
 * Declarations.
 */

namespace my {

struct WindowActions {
  bool close{false};
  bool resetSize{false};
  bool resize{false};
  bool pauseResume{false};
};

class WindowHandler {
public:
  WindowHandler();
  WindowHandler(const WindowHandler&) = delete;
  WindowHandler(WindowHandler&&) = delete;
  WindowHandler operator=(const WindowHandler&) = delete;
  WindowHandler operator=(WindowHandler&&) = delete;
  ~WindowHandler() noexcept;

  auto getWindow() -> GLFWwindow*;
  auto getActions() const -> const WindowActions&;
  auto getWidth() const -> int;
  auto getHeight() const -> int;
  auto isActive() const -> bool;
  auto close() -> void;
  auto resetSize() -> void;
  auto preRender() -> void;
  auto postRender() -> void;
  auto resetActions() -> void;

private:
  const int _initialWidth{400};
  const int _initialHeight{400};
  const char* _title{"World 3D"};
  const int glVersionMajor{3};
  const int glVersionMinor{3};
  GLFWwindow* _window;
  WindowActions _actions{};
  int _width;
  int _height;
  
  static auto onKeyGLFW(
    GLFWwindow* window, int key, int scancode, int action, int mods
  ) -> void;
  auto onKey(int key, int action, int mods) -> void;
};

} // namespace my

#endif // WINDOW_HXX
