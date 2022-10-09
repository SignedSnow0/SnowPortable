#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout (set = 0, binding = 0) uniform Camera {
    mat4 View;
    mat4 Projection;
} camera;

layout (push_constant) uniform Model {
    mat4 Model;
} model;

void main() {
    gl_Position = camera.Projection * camera.View * model.Model * vec4(inPosition, 1.0);
    outColor = inColor;
}