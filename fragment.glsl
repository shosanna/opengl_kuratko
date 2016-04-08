#version 150

uniform vec3 triangleColor;
out vec4 outColor;
in vec3 Color;
in vec2 Texcoord;

uniform sampler2D kuratko;
uniform sampler2D background;
uniform float selector;

void main()
{
  vec4 k = texture(kuratko, Texcoord);
  vec4 bg = texture(background, Texcoord);
  outColor = mix(k, bg, selector) * vec4(Color, 0.1);
}
