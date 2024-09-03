#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(set = 0, binding = 1) uniform HUDParUniformBufferObject {
    float alpha;
    bool visible;
} hudParUbo;


void main() {
    if (hudParUbo.visible) {
        float alpha = hudParUbo.alpha;
        outColor = alpha * texture(tex, fragTexCoord);
    } else {
        outColor = vec4(0.0);
    }
    
}
