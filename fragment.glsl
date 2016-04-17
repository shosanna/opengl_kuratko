#version 150

out vec4 outColor;
in vec3 Color;

void main()
{
  outColor = vec4(Color, 0.5);
}
