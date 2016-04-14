#version 150

uniform vec3 triangleColor;
out vec4 outColor;
in vec3 Color;

void main()
{
  outColor = vec4(Color, 0.1);
}
