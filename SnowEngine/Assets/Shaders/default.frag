#version 450

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform SceneSettings {
    vec4 ambientColor;
} sceneSettings;

layout (set = 1, binding = 0) uniform sampler2D albedo;
layout (set = 1, binding = 1) uniform sampler2D metallicRoughness;  
layout (set = 1, binding = 2) uniform sampler2D normalMap;

void main() {
    //material
    vec3 albedoColor = texture(albedo, inUV).rgb;

    vec3 result = albedoColor * sceneSettings.ambientColor.rgb;

    outColor = vec4(albedoColor,  1.0);
}