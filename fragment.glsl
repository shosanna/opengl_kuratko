#version 150

uniform vec3 triangleColor;
out vec4 outColor;
in vec3 Color;
in vec2 Texcoord;

uniform sampler2D pejsek;
uniform sampler2D kocicka;
uniform float selector;

void main()
{
  vec4 haf = texture(pejsek, Texcoord);
  vec4 mnau = texture(kocicka, Texcoord);
  outColor = mix(haf, mnau, selector) * vec4(Color, 0.1);
}
