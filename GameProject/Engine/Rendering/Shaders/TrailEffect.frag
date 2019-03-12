#version 420



in vec3 fragColor;

out vec4 finalColor;

in float fragAlpha;

void main()
{
    finalColor = vec4(fragColor, fragAlpha);
}