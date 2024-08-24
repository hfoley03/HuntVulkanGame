#version 450
#extension GL_ARB_separate_shader_objects : enable

// Inputs from the vertex shader
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragTangent;
layout(location = 2) in vec3 fragBitangent;
layout(location = 3) in vec3 fragNorm;
layout(location = 4) in vec2 fragUV;

// Output color
layout(location = 0) out vec4 outColor;

// Uniform buffers
layout(set = 0, binding = 0) uniform BlinnUniformBufferObject {
    vec3 lightDir;
    vec4 lightColor;
    vec3 eyePos;
} gubo;

layout(set = 1, binding = 2) uniform BlinnParUniformBufferObject {
    float Pow;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D tex;          // Base texture
layout(set = 1, binding = 3) uniform sampler2D normalMap;    // Normal map texture

void main() {
    // Sample the normal map and convert from [0, 1] to [-1, 1] range
    vec3 normalMapSample = texture(normalMap, fragUV).rgb * 2.0 - 1.0;

    // Create TBN matrix (Tangent, Bitangent, Normal)
    vec3 T = normalize(fragTangent);
    vec3 B = normalize(fragBitangent);
    vec3 N = normalize(fragNorm);
    mat3 TBN = mat3(T, B, N);

    // Transform normal from tangent space to world space
    vec3 Norm = normalize(TBN * normalMapSample);

    // Lighting calculations (Blinn-Phong)
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 lightDir = normalize(gubo.lightDir);
    vec3 lightColor = gubo.lightColor.rgb;

    // Diffuse component
    vec3 Diffuse = texture(tex, fragUV).rgb * 0.975f * max(dot(Norm, lightDir), 0.0);
    
    // Specular component (Blinn-Phong specular)
    vec3 halfwayDir = normalize(lightDir + EyeDir);
    vec3 Specular = vec3(pow(max(dot(Norm, halfwayDir), 0.0), mubo.Pow));
    
    // Ambient component
    vec3 Ambient = texture(tex, fragUV).rgb * 0.025f;
    
    // Final color
    vec3 col = (Diffuse + Specular) * lightColor + Ambient;
    
    outColor = vec4(col, 1.0f);
}
