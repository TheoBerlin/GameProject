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
     /*
        Ambient
    */
    vec3 ambient = dirLight.color_intensity.rgb * 0.2;
    /*
        Diffuse
    */
    vec3 diffuse = dirLight.color_intensity.rgb;
    diffuse *= max(-dot(fragNormal, normalize(dirLight.direction.xyz)), 0.0);

    /*
        Specular
    */
    vec3 specular;
    vec3 lightReflect = normalize(reflect(dirLight.direction.xyz, fragNormal));
    vec3 VertexToEye = normalize(camPos - fragPos);
    float specularFactor = dot(VertexToEye, lightReflect);
        if (specularFactor > 0) {
            specularFactor = pow(specularFactor, mat.ks_f.a);
            specular = dirLight.color_intensity.rgb * specularFactor;
        }
    
    texColor = min(texColor * dirLight.color_intensity.a, 1.0f);
    vec3 phong = min(texColor * (specular + ambient + diffuse), 1.0f);

    finalColor = vec4(phong, 1.0f);
}