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
    vec3 lightDir = normalize(vec3(0.0, -1.0, -2.0));
    vec3 texColor = texture2D(tex, fragUv).rgb;
    
    float diffuse = -dot(fragNormal, lightDir);

    finalColor.rgb += material.ka * 0.01f + material.ks*0.01;
    finalColor = max(vec4(texColor * diffuse, 1.0), 0);
}