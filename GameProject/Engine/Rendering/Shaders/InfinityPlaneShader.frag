#version 420

in vec2 fragScale;
in vec3 fragNormal;
in vec2 fragUv;
in vec3 fragPos;
in vec4 fragLightPos;

layout(std140) uniform WallPoints
{
    vec4 points[100];
    int size;
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

out vec4 finalColor;

uniform sampler2D tex;
uniform sampler2D shadowTex;
uniform vec3 camPos;

bool IsFloor(vec3 p)
{
    int i;
    int j = wallPoints.size-1;
    bool isOdd = false;
    for(i = 0; i < wallPoints.size; i++) {
        vec3 w2 = wallPoints.points[i].xyz;
        vec3 w1 = wallPoints.points[j].xyz;
        // Check if the horizontal line, which p.z spans, is intersecting the line between w1 and w2.
        if((w2.z < p.z && w1.z >= p.z) || (w1.z < p.z && w2.z >= p.z)) {
            // Check if the point is not the left or the right side of the line.
            if(w2.x + (w1.x-w2.x)*(p.z + w2.z)/(w1.z-w2.z) < p.x) {
                isOdd = !isOdd;
            }
        }
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
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    if(IsFloor(fragPos) == false)
        discard;

    vec2 limit = vec2(fragUv.x * fragScale.x, fragUv.y * fragScale.y);
    limit = vec2(limit.x / 10.0, limit.y / 5.0);

    vec3 texColor = texture2D(tex, vec2(mod(limit.x, 1.0), mod(limit.y, 1.0))).rgb;
    //vec3 texColor = texture2D(tex, fragUv).rgb;
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
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;

    finalColor = vec4(lighting, /*smoothstep(0.0, 200.0, length(fragPos))*/, 1.0);
}