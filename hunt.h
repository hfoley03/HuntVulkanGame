#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "generateScope.hpp"
#include "gameParameters.h"
#include "gameStructs.h"
#include "utils.hpp"
#include "gameObjects.hpp"
#include <cstdlib> 
#include <ctime>
#include <string>
#include <unistd.h>

class HuntGame : public BaseProject {
	private:
	
	DescriptorSetLayout DSLGlobal, DSLBlinn, DSLOren, DSLEmission, DSLHUD, DSLskyBox, DSLNMap;
	VertexDescriptor VDBlinn, VDOren, VDEmission, VDHUD, VDskyBox, VDNMap;
	Pipeline PBlinn, POren, PEmission, PHUD, PskyBox, PNMAp;

    TextMaker txt, timeTxt, animalTxt;
    
    Model MAx,Msun, Mmoon, Mground, MCrosshair, MScope, MskyBox, MMenuScreen, MGun, MBall, MTower;
	Model MVegRocks[12], MStructures[4], MAnimals[10];

	glm::vec3 towerPos = glm::vec3(-23.0f, 0.0f, 24.0f);

	DescriptorSet DSGlobal, DSAx, DSCrosshair, DSScope, DSsun, DSmoon, DSground, DSskyBox, DSGun, DSMenuScreen, DSTower;
	DescriptorSet DSAnimals[NANIMAL], DSVegRocks[NVEGROCK], DSStructures[NSTRUCTURES], DSBalls[NBALLS];

	Texture T1, Tgreen, Tanimal, TGun, TGrass, Tsun, Tmoon, Tground, TCrosshair, TskyBox, Tstars, TMenuScreen, TScope;
	Texture TTowerNMap;
	Texture TTowerDiff;
	Texture TStructures[4];

	VkFilter magFilter = VK_FILTER_NEAREST;
	VkFilter minFilter = VK_FILTER_NEAREST;
	VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	VkBool32 anisotropyEnable = VK_FALSE;
	float maxAnisotropy = 1;
	float maxLod = 0;
	int mmm = 0;

	int currScene = STARTMENU;
	int aliveAnimals = NANIMAL;
	float startTime = 0.0f;
	float currTime = 0.0f;
	float lastTime = 0.0f;
	int currTimeIndex = 0;
	int currAnimalIndex = 0;
	
	// Camera Parameters
	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
	glm::vec3 PlayerPos = glm::vec3(0.0, 0.1, 5.0);
	glm::mat4 ViewMatrix;
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;	
	float Ar;
	float cameraZoom = 50.0f;
	float zoomInAngle = 20.0f;
	float zoomOutAngle = 50.0f;

	Ray ray;
	bool isTorchOn = false;

	std::vector<SingleText> outText;		// Menu screen
	std::vector<SingleText> outTimeText;	// Time counter
	std::vector<SingleText> outAnimalText;	// Animal counter


	std::vector<GameObject> balls;
	std::vector<GameObject> vegRocks; 
	std::vector<GameObject> structures;
	std::vector<LiveAnimal> animals;

	void setWindowParameters(); 
	void onWindowResize(int w, int h); 
	void localInit(); 
	void pipelinesAndDescriptorSetsInit();
	void pipelinesAndDescriptorSetsCleanup();
	void localCleanup(); 
	void shootGun();
	void updateLiveAnimals(float deltaT);
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage);
	void checkPlayerObjectCollisions(glm::vec3& playerPos, glm::vec3&  firstPlayerPos, glm::vec3& lastPlayerPos);
	void updatePlayerPos(float& CamPitch, float& CamYaw, float deltaT, glm::vec3 m, glm::vec3 r, float ROT_SPEED);
	void handleInput(float cTime, float tTime);
	void updateUniformBuffer(uint32_t currentImage);
};





bool isDebugMode = false;	// Debug mode
float dayCyclePhase;		// Starting moment of the day

