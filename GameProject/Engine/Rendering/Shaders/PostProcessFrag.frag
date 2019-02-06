#version 420

out vec4 finalColor;

uniform sampler2D tex;

in vec2 fragUV;

void main()
{   
    vec4 fragColor = texture2D(tex, vec2(fragUV.x, 1.0 - fragUV.y));
    finalColor = fragColor;
}