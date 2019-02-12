#version 420

in vec4 coord;

uniform vec2 pos;

out vec2 texCoord;

void main()
{
    texCoord.xy = coord.zw;
    gl_Position = vec4(pos + coord.xy, 0.0, 1.0); 
}