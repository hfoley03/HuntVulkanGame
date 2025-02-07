// UNIFORM BUFFERS
#include <glm/glm.hpp>

struct BlinnUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct BlinnMatParUniformBufferObject {
	alignas(4) float Power;
	alignas(4) float scaleUV;
};

struct OrenUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OrenMatParUniformBufferObject {
	alignas(4)  float Roughness;
	alignas(4) 	float scaleUV;
};

struct NMapUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct NMapMatParUniformBufferObject {
	alignas(4)  float Roughness;
	alignas(4) 	float scaleUV;
};

struct EmissionUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct EmissionParUniformBufferObject {
	alignas(4) float temperature;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 dayLightColor;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 nightLightColor;
	alignas(4) float ambient;
	alignas(16) glm::vec3 pointLightColor;
	alignas(16) glm::vec3 userDir;
	alignas(4) float gFactor;
	alignas(4) float beta;
	alignas(4) float cIn;
	alignas(4) float cOut;
};

struct SkyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct SkyBoxParUniformBufferObject {
	alignas(4) float daytime;
};

struct HUDParUniformBufferObject {
	alignas(4) float alpha;
	alignas(4) bool visible;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

// VERTEX STRUCTURES

struct BlinnVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct OrenVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct NMapVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
	glm::vec4 tan;

};

struct EmissionVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

struct HUDVertex {
    glm::vec2 pos;
    glm::vec2 UV;
};

struct skyBoxVertex {
	glm::vec3 pos;
};