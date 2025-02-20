#include "camera.hxx"

auto my::Camera::moveX(float) -> void {}
auto my::Camera::moveY(float) -> void {}
auto my::Camera::moveZ(float) -> void {}
auto my::Camera::move(float, float, float) -> void {}
auto my::Camera::roll(float) -> void {}
auto my::Camera::yaw(float) -> void {}
auto my::Camera::pitch(float) -> void {}

auto my::Camera::getProjectionMatrix() -> glm::mat4& {
  return _projectionMatrix;
}

auto my::Camera::getViewMatrix() -> glm::mat4& {
  return _viewMatrix;
}
