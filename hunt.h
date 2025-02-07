#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
// #include "modules/generateScope.h"

#include "utils.hpp"
#include "objectInstances.hpp"
#include <cstdlib> 
#include <ctime>
#include <string>
#include <unistd.h>

// THE NUMBER OF INSTANCES OF ANIMALS, VEGITATION/ROCKS, STRUCTURES
#define NANIMAL 40
#define NVEGROCK 80
#define NSTRUCTURES 137
#define NBALLS 1

// GAME SCENES
#define NUMSCENES 4
#define STARTMENU 0
#define MATCH 1
#define GAMEWIN 2
#define GAMEOVER 3

// GAME PARAMETERS
#define GAMEDURATION 120	// seconds
#define UPDATETIME 10.0f	// seconds

// MOVEMENTS
#define ZOUT_ROT_SPEED glm::radians(120.0f);
#define ZIN_ROT_SPEED glm::radians(60.0f);

// LIGHTS PARAMETERS
#define G_FACTOR 1.0f
#define BETA 0.75f
#define C_IN 0.99
#define C_OUT 0.97



// UNIFORM BUFFERS

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

bool isDebugMode = false;	// Debug mode
float dayCyclePhase;		// Starting moment of the day

// TEXTS
std::vector<SingleText> outText;		// Menu screen
std::vector<SingleText> outTimeText;	// Time counter
std::vector<SingleText> outAnimalText;	// Animal counter


std::vector<Instance> balls;
std::vector<Instance> vegRocks; 
std::vector<Instance> structures;
std::vector<Instance> animals;