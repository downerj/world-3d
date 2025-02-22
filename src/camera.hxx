#ifndef CAMERA_HXX
#define CAMERA_HXX

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace my {

class Camera {
public:
  Camera();
  Camera(float fovy, float aspect, float zNear, float zFar);

  auto moveX(float dx) -> void;
  auto moveY(float dy) -> void;
  auto moveZ(float dz) -> void;
  auto move(float dx, float dy, float dz) -> void;
  auto roll(float dax) -> void;
  auto yaw(float day) -> void;
  auto pitch(float daz) -> void;
  auto getProjectionMatrix() const -> const glm::mat4&;
  auto getViewMatrix() const -> const glm::mat4&;
  auto getProjectionMatrixPointer() const -> const glm::mat4::value_type*;
  auto getViewMatrixPointer() const -> const glm::mat4::value_type*;

private:
  glm::mat4 _projectionMatrix{1.};
  glm::mat4 _viewMatrix{1.};
};

} // namespace my

#endif // CAMERA_HXX
