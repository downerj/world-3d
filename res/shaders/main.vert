#version 330

out vec3 vertexColor;
const vec2 points[3] = vec2[3](
  vec2(1., -1.),
  vec2(-1., -1.),
  vec2(0., 1.)
);
const vec3 colors[3] = vec3[3](
  vec3(1., 0., 0.),
  vec3(0., 1., 0.),
  vec3(0., 0., 1.)
);

void main() {
  gl_Position = vec4(points[gl_VertexID], 0., 1.);
  vertexColor = colors[gl_VertexID];
}
