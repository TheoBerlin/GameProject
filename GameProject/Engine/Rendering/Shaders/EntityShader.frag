#version 420

in vec3 fragNormal;
in vec2 fragUv;

layout(std140) uniform Material
{
    vec3 ka;
    vec3 ks;
} material;

out vec4 finalColor;

uniform sampler2D tex;

void main()
{
    vec3 texColor = texture2D(tex, fragUv).rgb;
    finalColor = vec4(texColor + 0.0*vec3(0.5, 0.0, 0.0), 1.0);
    
    float s = step(0.5, fragUv.x);
    finalColor = vec4(texColor, 1.0) + vec4(fragUv, 0.0, 1.0)*0.01 + (1.0 - s)*vec4((fragNormal+1.0)/2.0, 1.0)*0.01;

    finalColor.rgb += material.ka*0.01 + material.ks*0.01;
    finalColor = min(finalColor, 1.0);
}