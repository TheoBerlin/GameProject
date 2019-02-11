#version 420

in vec3 fragNormal;
in vec2 fragUv;
in vec3 fragPos;

layout(std140) uniform Material
{
    vec4 kd;
    vec4 ks_f;
} mat;

layout(std140) uniform DirectionalLight
{
    vec4 direction;
    vec4 color_intensity;
} dirLight;

out vec4 finalColor;

uniform sampler2D tex;
uniform vec3 camPos;


void main()
{
    vec3 texColor = texture2D(tex, fragUv).rgb;

    float diffuse = -dot(fragNormal, normalize(dirLight.direction.xyz));

    vec3 lightReflect = normalize(reflect(dirLight.direction.xyz, fragNormal));
    vec3 VertexToEye = normalize(camPos - fragPos);
    vec3 specular;
    float specularFactor = dot(VertexToEye, lightReflect);
        if (specularFactor > 0) {
            specularFactor = pow(specularFactor, 10.0f);
            specular = dirLight.color_intensity.rgb * specularFactor;
        }
    

    //finalColor.rgb += material.ka + material.ks*0.01;
    /*
        Apply sky color(ambient)
    */
    texColor *= dirLight.color_intensity.rgb;
    texColor = min(texColor * dirLight.color_intensity.a, 1.0f);

    finalColor = max(vec4(texColor * (diffuse + specular), 1.0f), 0.0f);
}