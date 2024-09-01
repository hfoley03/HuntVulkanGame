#version 450
#extension GL_ARB_separate_shader_objects : enable

// this defines the variable received from the Vertex Shader
// the locations must match the one of its out variables
layout(location = 0) in vec2 fragUV;

// This defines the color computed by this shader. Generally is always location 0.
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D tex;

layout(set = 0, binding = 2) uniform EmissionParUniformBufferObject {
	float temperature;
} eubo;

// The main shader, implementing a simple Blinn + Lambert + constant Ambient BRDF model
// The scene is lit by a single Spot Light
void main() {

	float temp = eubo.temperature;
	if (temp == -2.0f) {
		vec3 Emit = texture(tex, fragUV).rgb;
		outColor = vec4(Emit, 1.0f);
	} else {
		
		float r = 1.0f;
		float g = 0.2f + 0.8f * abs(temp);
		float b = abs(temp);

		outColor = vec4(r, g, b, 1.0f);
	}
}