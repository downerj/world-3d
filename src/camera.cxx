#include "camera.hxx"

#include <glm/gtc/type_ptr.hpp>

my::Camera::Camera() {}

my::Camera::Camera(float fovy, float aspect, float zNear, float zFar)
: _projectionMatrix{glm::perspective(fovy, aspect, zNear, zFar)},
  _fovy{fovy}, _aspect{aspect}, _zNear{zNear}, _zFar{zFar} {}

auto my::Camera::setAspectRatio(float aspect) -> void {
  _aspect = aspect;
  _projectionMatrix = glm::perspective(_fovy, _aspect, _zNear, _zFar);
}

auto my::Camera::setX(float x) -> void {
  _viewMatrix[3][0] = -x;
}

auto my::Camera::setY(float y) -> void {
  _viewMatrix[3][1] = -y;
}

auto my::Camera::setZ(float z) -> void {
  _viewMatrix[3][2] = -z;
}

auto my::Camera::setPosition(float x, float y, float z) -> void {
  _viewMatrix[3][0] = -x;
  _viewMatrix[3][1] = -y;
  _viewMatrix[3][2] = -z;
}

auto my::Camera::moveX(float dx) -> void {
  _translateVector.x = -dx;
}

auto my::Camera::moveY(float dy) -> void {
  _translateVector.y = -dy;
}

auto my::Camera::moveZ(float dz) -> void {
  _translateVector.z = -dz;
}

auto my::Camera::move(float dx, float dy, float dz) -> void {
  _translateVector.x = -dx;
  _translateVector.y = -dy;
  _translateVector.z = -dz;
}

auto my::Camera::roll(float) -> void {}
auto my::Camera::yaw(float) -> void {}
auto my::Camera::pitch(float) -> void {}

auto my::Camera::update() -> void {
  _viewMatrix = glm::translate(_viewMatrix, _translateVector);
}

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
