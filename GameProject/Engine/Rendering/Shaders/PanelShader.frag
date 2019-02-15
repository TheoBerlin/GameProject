#version 420

in vec2 texCoord;

uniform sampler2D tex;
uniform vec4 color;

out vec4 finalColor;

void main()
{
    finalColor = texture(tex, texCoord)*color;
}