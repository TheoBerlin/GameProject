#version 420

in vec4 coord;

out vec2 texCoord;

void main()
{
    texCoord = coord.zw;
    gl_Position = vec4(texCoord.xy, 0.0, 1.0); 
}