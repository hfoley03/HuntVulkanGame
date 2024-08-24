#version 450
#extension GL_ARB_separate_shader_objects : enable

// The attributes associated with each vertex.
layout(location = 0) in vec3 inPosition;   // Vertex position
layout(location = 1) in vec3 inNorm;       // Vertex normal
layout(location = 2) in vec2 inUV;         // Vertex UV coordinates
layout(location = 3) in vec3 inTangent;    // Vertex tangent

// Output to the Fragment Shader
layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragTangent;
layout(location = 2) out vec3 fragBitangent;
layout(location = 3) out vec3 fragNorm;
layout(location = 4) out vec2 fragUV;

// Uniform buffers for transformation matrices
layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;   // Model-View-Projection matrix
    mat4 mMat;     // Model matrix
    mat4 nMat;     // Normal matrix (inverse transpose of model matrix)
} ubo;

void main() {
    // Compute the clipping coordinates
    gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);

    // Compute the position in world space
    fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;

    // Transform the normal and tangent to world space
    fragNorm = normalize((ubo.nMat * vec4(inNorm, 0.0)).xyz);
    fragTangent = normalize((ubo.nMat * vec4(inTangent, 0.0)).xyz);

    // Compute the bitangent as the cross product of the normal and tangent
    fragBitangent = normalize(cross(fragNorm, fragTangent));

    // Pass UV coordinates to the fragment shader
    fragUV = inUV;
}
