#version 420

in vec2 texCoord;

uniform sampler2D tex;

out vec4 finalColor;

void main()
{
    finalColor = texture(tex, texCoord);
    //finalColor = mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(finalColor.xyz, 1.0), finalColor.w);
}