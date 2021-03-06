#version 150

in vec2 position;
in vec3 color;
in vec2 texcoord;

out vec3 Color;
out vec2 Texcoord;

void main()
{
  gl_Position = vec4(position, 0.5, 1.0);
  Color = color;
  Texcoord = texcoord;
}
