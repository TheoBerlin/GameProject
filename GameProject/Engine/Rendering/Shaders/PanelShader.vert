#version 420

in vec4 coord;

out vec2 texCoord;

uniform vec2 size;
uniform vec2 pos;

void main()
{
    texCoord.xy = coord.zw;
    gl_Position = vec4(pos + coord.xy*size, 0.0, 1.0); 
}