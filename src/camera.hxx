#ifndef CAMERA_HXX
#define CAMERA_HXX

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace my {

class Camera {
public:
  Camera(float fovy, float aspect, float zNear, float zFar);

  auto setAspectRatio(float aspect) -> void;
  auto setAspectRatio(int width, int height) -> void;
  auto setX(float x) -> void;
  auto setY(float y) -> void;
  auto setZ(float z) -> void;
  auto setPosition(float x, float y, float z) -> void;
  auto moveX(float dx) -> void;
  auto moveY(float dy) -> void;
  auto moveZ(float dz) -> void;
  auto move(float dx, float dy, float dz) -> void;
  auto roll(float dax) -> void;
  auto yaw(float day) -> void;
  auto pitch(float daz) -> void;
  auto update() -> void;
  auto getProjectionMatrix() const -> const glm::mat4&;
  auto getViewMatrix() const -> const glm::mat4&;
  auto getProjectionMatrixPointer() const -> const glm::mat4::value_type*;
  auto getViewMatrixPointer() const -> const glm::mat4::value_type*;

private:
  glm::mat4 _projectionMatrix{1.};
  glm::mat4 _viewMatrix{1.};
  glm::vec3 _translateVector{};
  float _fovy;
  float _aspect;
  float _zNear;
  float _zFar;
};

} // namespace my

#endif // CAMERA_HXX
