#include "camera.hxx"

#include <glm/gtc/type_ptr.hpp>

my::Camera::Camera() {}

my::Camera::Camera(float fovy, float aspect, float zNear, float zFar)
: _projectionMatrix{glm::perspective(fovy, aspect, zNear, zFar)} {}

auto my::Camera::moveX(float) -> void {}
auto my::Camera::moveY(float) -> void {}
auto my::Camera::moveZ(float) -> void {}
auto my::Camera::move(float, float, float) -> void {}
auto my::Camera::roll(float) -> void {}
auto my::Camera::yaw(float) -> void {}
auto my::Camera::pitch(float) -> void {}

auto my::Camera::getProjectionMatrix() const -> const glm::mat4& {
  return _projectionMatrix;
}

auto my::Camera::getViewMatrix() const -> const glm::mat4& {
  return _viewMatrix;
}

auto my::Camera::getProjectionMatrixPointer() const
-> const glm::mat4::value_type* {
  return glm::value_ptr(_projectionMatrix);
}

auto my::Camera::getViewMatrixPointer() const -> const glm::mat4::value_type* {
  return glm::value_ptr(_viewMatrix);
}
