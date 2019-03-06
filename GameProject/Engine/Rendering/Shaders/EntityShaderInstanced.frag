#version 420

in vec3 fragNormal;
in vec2 fragUv;
in vec3 fragPos;
in vec4 fragLightPos;

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

layout(location = 0) out vec4 finalColor;

uniform sampler2D tex;
uniform sampler2D shadowTex;
uniform vec3 camPos;

float ShadowCalculation(vec4 fragLightSpace)
{
    float bias = 0.000002;
    // perform perspective divide
    vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowTex, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}  

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
    //vec3 diffuse = dirLight.color_intensity.rgb  * dirLight.color_intensity.a;
    float diffuse = max(-dot(normalize(fragNormal), normalize(dirLight.direction.xyz)), 0.0);

    /*
        Specular
    */
    float specular;
    vec3 lightReflect = normalize(reflect(dirLight.direction.xyz, fragNormal));
    vec3 VertexToEye = normalize(camPos - fragPos);
    float specularFactor = dot(VertexToEye, lightReflect);
        if (specularFactor > 0) {
            specular = pow(specularFactor, mat.ks_f.a);
        }
    
    texColor = min(texColor, 1.0f);

    /*
		Shadow
	*/
	float shadow = ShadowCalculation(fragLightPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;

    finalColor = vec4(lighting, 1.0);
}