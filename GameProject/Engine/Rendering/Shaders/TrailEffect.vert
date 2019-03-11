#version 420

layout(location = 0) in vec4 last;
layout(location = 1) in vec4 current;
layout(location = 2) in mat4 next;
//layout(location = 3) in float texOff;
//layout(location = 4) in vec3 baryCentric;

uniform mat4 proj, view;
uniform float width;
uniform vec2 viewPort;

const float pi = 3.141592653589793;

out vec3 fragColor;

vec4 transform(vec3 coord){
    return proj * view * vec4(coord, 1.0);
}

vec2 project(vec4 device){
    vec3 device_normal = device.xyz/device.w;
    vec2 clip_pos = (device_normal*0.5+0.5).xy;
    return clip_pos * viewport;
}

vec4 unproject(vec2 screen, float z, float w){
    vec2 clip_pos = screen/viewport;
    vec2 device_normal = clip_pos*2.0-1.0;
    return vec4(device_normal*w, z, w);
}

float estimateScale(vec3 position, vec2 sPosition){
    vec4 view_pos = view * vec4(position, 1.0);
    vec4 scale_pos = view_pos - vec4(normalize(view_pos.xy)*width, 0.0, 0.0);
    vec2 screen_scale_pos = project(proj * scale_pos);
    return distance(sPosition, screen_scale_pos);
}

void main()
{
    vec2 sLast = project(transform(last.xyz));
    vec2 sNext = project(transform(next.xyz));

    vec4 dCurrent = transform(current.xyz);
    vec2 sCurrent = project(dCurrent);

    vec2 normal1 = normalize(sLast - sCurrent);
    vec2 normal2 = normalize(sCurrent - sNext);
    vec2 normal = normalize(normal1 + normal2);
    float off = current.w;

    float angle = atan(normal.x, normal.y)+pi*0.5;
    vec2 dir = vec2(sin(angle), cos(angle))*off;
    float scale = estimateScale(current.xyz, sCurrent);
    vec2 pos = sCurrent + dir*scale;

    gl_Position = unproject(pos, dCurrent.z, dCurrent.w);
}