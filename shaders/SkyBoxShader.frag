#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D skybox;
layout(binding = 2) uniform sampler2D stars;
layout(binding = 3) uniform SkyBoxParUniformBufferObject {
	float dayTime;
} dtubo;


void main() {
	float time = 0.5*(dtubo.dayTime + 1);
	float yaw = -(atan(fragTexCoord.x, fragTexCoord.z)/6.2831853+0.5);
	float pitch = -(atan(fragTexCoord.y, sqrt(fragTexCoord.x*fragTexCoord.x+fragTexCoord.z*fragTexCoord.z))/3.14159265+0.5);
	// outColor = texture(skybox, vec2(yaw, pitch))*0.9+texture(stars, vec2(yaw, pitch))*0.1;
	outColor = (1-time) * texture(skybox, vec2(yaw, pitch)) + time * vec4(0.53f, 0.81f, 0.92f, 1.0f);
	// outColor = (1-time) * texture(skybox, vec2(yaw, pitch)) + time * texture(stars, vec2(yaw, pitch));
}