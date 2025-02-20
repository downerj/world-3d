#ifndef CAMERA_HXX
#define CAMERA_HXX

#include <glm/glm.hpp>

namespace my {

class Camera {
public:
  auto moveX(float dx) -> void;
  auto moveY(float dy) -> void;
  auto moveZ(float dz) -> void;
  auto move(float dx, float dy, float dz) -> void;
  auto roll(float dax) -> void;
  auto yaw(float day) -> void;
  auto pitch(float daz) -> void;
  auto getProjectionMatrix() -> glm::mat4&;
  auto getViewMatrix() -> glm::mat4&;

private:
  glm::mat4 _projectionMatrix;
  glm::mat4 _viewMatrix;
};

} // namespace my

#endif // CAMERA_HXX
