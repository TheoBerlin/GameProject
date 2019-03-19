#version 420

layout (location = 0) out vec4 colorAttachment0;
layout (location = 1) out vec4 colorAttachment1;

in vec3 droneColor;
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

struct PointLight
{
	vec4 position;
	vec4 intensity;

	float constant;
	float linear;
	float quadratic;
    float padding;
};

layout(std140) uniform LightBuffer
{
    PointLight pointLights[10];
    int nrOfPointLights;
    vec3 padding;
} lightBuffer;

uniform sampler2D tex;
uniform sampler2D shadowTex;
uniform sampler2D noiseTex;
uniform vec3 camPos;
uniform float time;
uniform bool isGlowing;

float ShadowCalculation(vec4 fragLightSpace)
{
    float bias = dirLight.color_intensity.a;
    // perform perspective divide
    vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowTex, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow and add PCF
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.ks_f.w);
    // attenuation
    float distanc    = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanc + light.quadratic * (distanc * distanc));    
    // combine results
    vec3 ambient  = vec3(0.1) * mat.kd.xyz;
    vec3 diffuse  = light.intensity.xyz * diff * mat.kd.xyz * light.intensity.w;
    vec3 specular = spec * mat.ks_f.xyz;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(camPos - fragPos);

    vec3 result = vec3(0.0f);

    for(int i = 0; i < lightBuffer.nrOfPointLights; i++) {
        result += CalcPointLight(lightBuffer.pointLights[i], normal, fragPos, viewDir);
    }

    vec3 texColor = texture2D(tex, fragUv).rgb;
     /*
        Ambient
    */
    vec3 ambient = dirLight.color_intensity.rgb * 0.2;
    /*
        Diffuse
    */
    //vec3 diffuse = dirLight.color_intensity.rgb  * dirLight.color_intensity.a;
    float diffuse = max(-dot(normal, normalize(dirLight.direction.xyz)), 0.0);

    /*
        Specular
    */
    float specular;
    vec3 lightReflect = normalize(reflect(dirLight.direction.xyz, normal));
    vec3 VertexToEye = normalize(camPos - fragPos);
    float specularFactor = dot(VertexToEye, lightReflect);
        if (specularFactor > 0) {
            specular = pow(specularFactor, mat.ks_f.a);
        }
    
    texColor = min(texColor, 1.0f);
    vec2 uv = fragUv;
    uv *= 10.0;
    uv.x += mod(time * 0.1, 1.0);
    vec3 noise = texture2D(noiseTex, uv).rgb;

    /*
		Shadow
	*/
	float shadow = ShadowCalculation(fragLightPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular * diffuse) + result) * texColor;
    lighting += noise * droneColor;
    colorAttachment0 = vec4(lighting, 1.0);
    
    if(isGlowing) {
        colorAttachment1 = vec4(texColor, 1.0);
        return;
     }
     colorAttachment1 = vec4(0.0, 0.0, 0.0, 0.0);
}