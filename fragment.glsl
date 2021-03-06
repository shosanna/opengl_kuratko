#version 150

uniform vec3 triangleColor;
out vec4 outColor;
in vec3 Color;
in vec2 Texcoord;

uniform sampler2D activeTex;

void main()
{
  vec4 tex = texture(activeTex, Texcoord);
  if(tex.a < 0.1)
    discard;
  outColor = tex * vec4(Color, 1.0);
}
