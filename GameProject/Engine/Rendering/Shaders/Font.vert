#version 420

in vec4 coord;

out vec2 texCoord;

uniform mat4 ortho;
uniform vec2 scale;
uniform vec2 pos;

void main()
{
    texCoord.xy = coord.zw;
    gl_Position = ortho*vec4(pos + coord.xy*scale, 0.0, 1.0); 
}