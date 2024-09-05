#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragUV;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

// Simple Perlin noise function (as a placeholder)
float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);

    // Some dummy gradient noise calculation
    return fract(sin(dot(i, vec3(12.9898, 78.233, 45.643))) * 43758.5453);
}

void main() {
    // Generate Perlin noise for height variation
    float height = noise(inPosition * 0.1) * 0.5;

    // Modify the y component of the vertex position without touching UVs
    vec3 modifiedPosition = vec3(inPosition.x, inPosition.y + height, inPosition.z);

    // Compute the final position in clip space
    gl_Position = ubo.mvpMat * vec4(modifiedPosition, 1.0);

    // Pass world-space position, transformed normal, and UV coordinates to fragment shader
    fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;
    fragNorm = (ubo.nMat * vec4(inNorm, 0.0)).xyz;
    fragUV = inUV;  // Leave UVs untouched to preserve the texture
}
