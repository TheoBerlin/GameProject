#version 420

in vec4 coord;

out vec2 texCoord;

void main()
{
    texCoord.x = coord.z;
    texCoord.y = 1.0 - coord.w;
    gl_Position = vec4(coord.xy, 0.0, 1.0); 
}