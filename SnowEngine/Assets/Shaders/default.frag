#version 450

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D albedo;

void main() {
    vec3 albedoColor = texture(albedo, inUV).rgb;

    outColor = vec4(albedoColor,  1.0);
}