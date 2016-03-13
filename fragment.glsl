#version 150

uniform vec3 triangleColor;
out vec4 outColor;
in vec3 Color;
in vec2 Texcoord;
uniform sampler2D tex;

void main()
{
  outColor = texture(tex, Texcoord) * vec4(Color, 0.1);
}
