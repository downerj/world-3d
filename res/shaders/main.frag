#version 330

#ifdef GL_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 vertexColor;

out vec4 fragColor;

void main() {
  fragColor = vec4(vertexColor, 1.);
}
