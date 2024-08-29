#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform UBO {
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo;

void main() {
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
}
