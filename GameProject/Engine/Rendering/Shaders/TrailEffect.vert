#version 420

/*
    Credit to pyalot on github https://github.com/pyalot/webgl-trails/blob/master/src/textured.shader#L25
*/

layout(location = 0) in vec4 previous;
layout(location = 1) in vec4 current;
layout(location = 2) in float alpha;
layout(location = 3) in float width;
layout(location = 4) in vec3 upVector;

uniform mat4 proj, view;
uniform vec3 trailColor;
uniform bool horizontal;

const float pi = 3.141592653589793;

out vec3 fragColor;
out vec3 fragPos;
out float fragAlpha;

vec4 transform(vec3 coord){
    return proj * view * vec4(coord, 1.0);
}

void main()
{
    fragColor = trailColor;
    fragAlpha = alpha;

    vec3 normal = normalize(current.xyz - previous.xyz);

    float off = current.w;

    vec3 pos;
    if(horizontal)
        pos = current.xyz + width * cross(upVector, normal) * off;
    else {
        pos = current.xyz + width * upVector * off;
    }

    fragPos = pos;

    gl_Position = transform(pos);
}