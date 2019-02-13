#version 420

in vec2 texCoord;

uniform sampler2D tex;
uniform vec4 color;

out vec4 finalColor;

void main()
{
    finalColor = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r)*color;
}