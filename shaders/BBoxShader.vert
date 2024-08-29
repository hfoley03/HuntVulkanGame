#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform UBO {
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

void main() {
    gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);
}
