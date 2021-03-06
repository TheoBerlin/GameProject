#version 420

in vec2 fragScale;
in vec3 fragNormal;
in vec2 fragUv;
in vec3 fragPos;
in vec4 fragLightPos;

layout(std140) uniform WallPoints
{
    vec4 points[96];
    int size;
    int groupSize;
} wallPoints;

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

out vec4 finalColor;

uniform sampler2D tex;
uniform sampler2D shadowTex;
uniform vec3 camPos;

bool InRoom(vec3 p, int size, int index)
{  
    int j = index + size - 1;
    bool isOdd = index > 0;
    for(int i = index; i < size+index; i++) {
        vec3 w2 = wallPoints.points[i].xyz;
        vec3 w1 = wallPoints.points[j].xyz;
        // Check if the horizontal line, which p.z spans, is intersecting the line between w1 and w2.
        if((w2.z < p.z && w1.z >= p.z) || (w1.z < p.z && w2.z >= p.z)) {
            // Check if the point is not the left or the right side of the line.
            if(((w1.x-w2.x)/(w1.z-w2.z)) * (p.z - w1.z) + w1.x > p.x) {
				isOdd = !isOdd;
            }
        }
        j = i;
    }
    return isOdd;
}

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
    // check whether current frag pos is in shadow and add PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowTex, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowTex, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    //shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow / 15.0;
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
    vec3 specular = vec3(1.0) * spec * mat.ks_f.xyz;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    int index = 0;
    int counter = 0;
    for (int i = 0; i < wallPoints.groupSize; i++)
    {
        if(InRoom(fragPos, int(wallPoints.points[index].w), index))
            counter++;
        index += int(wallPoints.points[index].w);
    }
    if (counter == wallPoints.groupSize)
        discard;

    vec3 viewDir = normalize(camPos - fragPos);

    vec3 result = vec3(0.0f);

    for(int i = 0; i < lightBuffer.nrOfPointLights; i++) {
        result += CalcPointLight(lightBuffer.pointLights[i], fragNormal, fragPos, viewDir);
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

    /*
		Shadow
	*/
	float shadow = ShadowCalculation(fragLightPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)+ result) * texColor;

    finalColor = vec4(lighting, 1.0 - smoothstep(0.0, 75.0, length(fragPos)));
}