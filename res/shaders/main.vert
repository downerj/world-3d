#version 330

in vec3 position;
in vec3 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vertexColor;

void main() {
  gl_Position = projection*view*model*vec4(position, 1.);
  // gl_Position = vec4(position, 1.);
  vertexColor = color;
}
