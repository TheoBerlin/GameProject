#version 420

in vec4 coord;

uniform vec2 pos;
uniform vec2 scale;

out vec2 texCoord;

void main()
{
    texCoord.x = coord.z;
    texCoord.y = 1.0 - coord.w;
    gl_Position = vec4(pos + coord.xy * scale, 0.0, 1.0); 
}