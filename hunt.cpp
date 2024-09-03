// This has been adapted from the Vulkan tutorial
// And from A10

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "utils.hpp"
#include <cstdlib> 

bool isDebugMode = false;

std::vector<SingleText> outText = {
	{2, {"Welcome to our hunting game!", "Press ENTER to start the game","",""}, 0, 0},
	{2, {"Let the hunt begin!","","",""}, 0, 0},
	{2, {"Game over","Press ENTER to start again","",""}, 0, 0},
};

// THE NUMBER OF INSTANCES OF ANIMALS, VEGITATION/ROCKS, STRUCTURES
#define NANIMAL 5 
#define NVEGROCK 73
#define NSTRUCTURES 141
#define NBALLS 1

// GAME SCENES
#define NUMSCENES 3
#define STARTMENU 0
#define MATCH 1
#define GAMEOVER 2


struct BlinnUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct BlinnMatParUniformBufferObject {
	alignas(4)  float Power;
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
};

struct SkyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct SkyBoxParUniformBufferObject {
	alignas(4) float daytime;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

struct BlinnVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct EmissionVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

struct HUDVertex {
    glm::vec2 pos;
    glm::vec2 UV;
};

struct BBoxVertex {
	glm::vec3 pos;
};

struct skyBoxVertex {
	glm::vec3 pos;
};

struct BoundingBox {
    glm::vec3 min;  // min corner
    glm::vec3 max;  // max corner
    BoundingBox() : min(0.0f), max(0.0f) {}
    BoundingBox(const glm::vec3& minCorner, const glm::vec3& maxCorner)
        : min(minCorner), max(maxCorner) {}
};


// class used to create instances of an object

class Instance {
public:
    glm::vec3 pos;      // Instance position
    int modelID;        // Model ID used to get the model of the object
    glm::vec3 scale;    // Scale
    float angle;        // Angle for rotation
    std::string desc;   // Description of the instance
    BoundingBox bbox;   // Bounding box for collision detection
    bool visible = true;
	float angle2 = 0.0f;


    Instance(const glm::vec3& position, int id, const glm::vec3& scl, float ang, const std::string& description)
        : pos(position), modelID(id), scale(scl), angle(ang), desc(description), bbox() {}

    void updateBoundingBox(const glm::vec3& modelMin, const glm::vec3& modelMax) {
    glm::vec3 scaledMin = modelMin * scale;
    glm::vec3 scaledMax = modelMax * scale;

    //  rotation todo

    bbox.min = pos + scaledMin;
    bbox.max = pos + scaledMax;
	}
};

void shoot(){
	std::cout << "shoot" << std::endl;
}


bool rayIntersectsBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const BoundingBox& box) {
    float tmin = (box.min.x - rayOrigin.x) / (rayDirection.x * -1.0);
    float tmax = (box.max.x - rayOrigin.x) / (rayDirection.x * -1.0);
    
    if (tmin > tmax) std::swap(tmin, tmax);
    
    float tymin = (box.min.y - rayOrigin.y) / (rayDirection.y * -1.0);
    float tymax = (box.max.y - rayOrigin.y) / (rayDirection.y * -1.0);
    
    if (tymin > tymax) std::swap(tymin, tymax);
    
    if ((tmin > tymax) || (tymin > tmax))
        return false;
    
    tmin = std::max(tmin, tymin);
    tmax = std::min(tmax, tymax);
    
    float tzmin = (box.min.z - rayOrigin.z) / rayDirection.z;
    float tzmax = (box.max.z - rayOrigin.z) / rayDirection.z;
    
    if (tzmin > tzmax) std::swap(tzmin, tzmax);
    
    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    
    return true;
}

bool rayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& sphereCenter, float sphereRadius, float& t0, float& t1) {
    glm::vec3 L = glm::vec3(sphereCenter.x, sphereCenter.y + 0.25f, sphereCenter.z) - rayOrigin;
    float tca = glm::dot(L, rayDirection);
    float d2 = glm::dot(L, L) - tca * tca;
    float radius2 = sphereRadius * sphereRadius;
    if (d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
}



struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

// calculate ray from the screen center (crosshair)
Ray calculateRayFromScreenCenter(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    glm::vec2 ndcCoords(0.0f, 0.0f); // Center of the screen in NDC

    //unproject the point both at the near plane and the far plane
    glm::vec4 rayStartNDC(ndcCoords, -1.0f, 1.0f); // Near plane (Z = -1 in NDC)
    glm::vec4 rayEndNDC(ndcCoords, 1.0f, 1.0f);    // Far plane (Z = 1 in NDC)

    glm::mat4 invVP = glm::inverse(projectionMatrix * viewMatrix);

    // NDC points to world space
    glm::vec4 rayStartWorld = invVP * rayStartNDC;
    glm::vec4 rayEndWorld = invVP * rayEndNDC;

    // convert from homogeneous to 3D coordinates
    rayStartWorld /= rayStartWorld.w;
    rayEndWorld /= rayEndWorld.w;
    Ray ray;
    ray.origin = glm::vec3(rayStartWorld); 
    ray.direction = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));  
    return ray;
}

glm::vec3 extractPlayerPositionFromViewMatrix(const glm::mat4& viewMatrix) {
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
    glm::vec3 playerPosition = glm::vec3(inverseViewMatrix[3]);
    return playerPosition;
}

bool checkCollision(glm::vec3 playerPos, float playerRad, glm::vec3 objectPos, float objectRad){

	glm::vec2 playXZ = glm::vec2(playerPos.x, playerPos.z);
	glm::vec2 objXZ = glm::vec2(objectPos.x, objectPos.z);

	float distSquared = glm::length(playerPos - objectPos);
	float radSummed = playerRad + objectRad;
	float radSummedSquared = radSummed * radSummed;

	return distSquared < radSummedSquared;
}

// void resolveCollision()

// MAIN ! 
class HuntGame : public BaseProject {
	protected:
	Ray ray;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal;	    // For Global values
	DescriptorSetLayout DSLBlinn;	    // For Blinn Objects
	DescriptorSetLayout DSLEmission;	// For Emission Objects
	DescriptorSetLayout DSLHUD;
	DescriptorSetLayout DSLBBox;
	DescriptorSetLayout DSLskyBox;	// For skyBox

	// Vertex formats
	VertexDescriptor VDBlinn;
	VertexDescriptor VDEmission;
	VertexDescriptor VDHUD;
	VertexDescriptor VDBBox;
	VertexDescriptor VDskyBox;

	// Pipelines [Shader couples]
	Pipeline PBlinn;
	Pipeline PEmission;
	Pipeline PHUD;
	Pipeline PBBox;
	Pipeline PskyBox;

	// Scenes and texts
    TextMaker txt;
    
    // Models
    Model MAx; // xyz axis object
    Model Msun;
	Model Mmoon;
    Model Mground;
    Model MCrosshair;
    Model MBBox;
	Model MskyBox;
	
	Model MGun;
    
    Model MBall;
    std::vector<Instance> balls;

	// we have 12 different models of different vegitiation and rocks, but we can have many more instances of these models
    Model MVegRocks[12];
    std::vector<Instance> vegRocks; // vector holding all instances of the vegrocks

    Model MStructures[4];
    std::vector<Instance> structures;

    Model MAnimals[10];
    std::vector<Instance> animals;

    
    // Descriptor Sets
    DescriptorSet DSGlobal, DSAx, DSCrosshair;
    DescriptorSet DSsun;
	DescriptorSet DSmoon;
    DescriptorSet DSground;
    DescriptorSet DSAnimals[NANIMAL];
    DescriptorSet DSVegRocks[NVEGROCK];
    DescriptorSet DSStructures[NSTRUCTURES];
    DescriptorSet DSBBox;
    DescriptorSet DSBalls[NBALLS];
	DescriptorSet DSskyBox;
	DescriptorSet DSGun;


   // Textures
    Texture T1, Tanimal, TGun, TGrass;
    Texture Tsun;
	Texture Tmoon;
    Texture Tground;
    Texture TStructures[4];
    Texture TCrosshair;
	Texture TskyBox, Tstars;

	 VkFilter magFilter = VK_FILTER_NEAREST;
	 VkFilter minFilter = VK_FILTER_NEAREST;
	 VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	 VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	 VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	 VkBool32 anisotropyEnable = VK_FALSE;
	 float maxAnisotropy = 1;
	 float maxLod = 0;
	 int mmm = 0;

	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
	glm::vec3 PlayerPos = glm::vec3(0.0, 0.1, 5.0);
	glm::mat4 ViewMatrix;
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;

	float Ar;

	// for visualisation of the bounding box only
	void createBoundingBoxModel(Model& MBBox, const BoundingBox& bbox, VertexDescriptor& VDBBox) {
	    int mainStride = sizeof(BBoxVertex); 

	    std::vector<BBoxVertex> bboxVertices = {
	        {bbox.min},  // Vertex 0: (min.x, min.y, min.z)
	        {{bbox.max.x, bbox.min.y, bbox.min.z}},  // Vertex 1: (max.x, min.y, min.z)
	        {{bbox.min.x, bbox.max.y, bbox.min.z}},  // Vertex 2: (min.x, max.y, min.z)
	        {{bbox.min.x, bbox.min.y, bbox.max.z}},  // Vertex 3: (min.x, min.y, max.z)
	        {{bbox.max.x, bbox.max.y, bbox.min.z}},  // Vertex 4: (max.x, max.y, min.z)
	        {{bbox.max.x, bbox.min.y, bbox.max.z}},  // Vertex 5: (max.x, min.y, max.z)
	        {{bbox.min.x, bbox.max.y, bbox.max.z}},  // Vertex 6: (min.x, max.y, max.z)
	        {bbox.max},  // Vertex 7: (max.x, max.y, max.z)
	    };
	    for (const auto& vertex : bboxVertices) {
	        std::vector<unsigned char> vertexData(mainStride, 0);
	        BBoxVertex* V_vertex = (BBoxVertex*)(&vertexData[0]);

	        V_vertex->pos = vertex.pos;

	        MBBox.vertices.insert(MBBox.vertices.end(), vertexData.begin(), vertexData.end());
	    }
	    std::vector<uint32_t> bboxIndices = {
	        0, 1,  1, 5,  5, 3,  3, 0,
	        2, 4,  4, 7,  7, 6,  6, 2,
	        0, 2,  1, 4,  5, 7,  3, 6
	    };
	    MBBox.indices.insert(MBBox.indices.end(), bboxIndices.begin(), bboxIndices.end());
	    MBBox.initMesh(this, &VDBBox);
	}
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A Hunting Game Called Hunt";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.53f, 0.81f, 0.92f, 1.0f};
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
				});
		DSLBlinn.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BlinnUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(BlinnMatParUniformBufferObject), 1}
				});
		DSLEmission.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(EmissionUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(EmissionParUniformBufferObject), 1}
				});
		DSLskyBox.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SkyBoxUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1},
					{3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(SkyBoxParUniformBufferObject), 1}
				});
		DSLHUD.init(this, {
    			{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}  // Crosshairs texture sampler
				});
		DSLBBox.init(this, {
    			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BlinnUniformBufferObject), 1} //BoundingBoxUniformBufferObject
				});

        
		// Vertex descriptors
		VDBlinn.init(this, {
				  {0, sizeof(BlinnVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BlinnVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BlinnVertex, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(BlinnVertex, UV),
				         sizeof(glm::vec2), UV}
				});
		VDEmission.init(this, {
				  {0, sizeof(EmissionVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(EmissionVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(EmissionVertex, UV),
				         sizeof(glm::vec2), UV}
				});

		VDskyBox.init(this, {
					{0, sizeof(skyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
					{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(skyBoxVertex, pos),
							sizeof(glm::vec3), POSITION}
				});

		VDHUD.init(this, {
				  {0, sizeof(HUDVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(HUDVertex, pos),
				         sizeof(glm::vec2), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(HUDVertex, UV),
				         sizeof(glm::vec2), UV}
				});

		VDBBox.init(this, {
				  {0, sizeof(BBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BBoxVertex, pos),
				         sizeof(glm::vec3), POSITION}
				});


          
		// Pipelines [Shader couples]
		PBlinn.init(this, &VDBlinn,  "shaders/BlinnVert.spv",    "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
		PEmission.init(this, &VDEmission,  "shaders/EmissionVert.spv",    "shaders/EmissionFrag.spv", {&DSLEmission});
		PskyBox.init(this, &VDskyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLskyBox});
		PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_BACK_BIT, false);
		PHUD.init(this, &VDHUD, "shaders/HUDVert.spv", "shaders/HUDFrag.spv", {&DSLHUD});
        PHUD.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, true);
        PBBox.init(this, &VDBBox, "shaders/BBoxVert.spv", "shaders/BBoxFrag.spv", {&DSLBBox});
        PBBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

		// Create models
        MAx.init(this, &VDBlinn, "models/axis.obj", OBJ);
		Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		Mmoon.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		Mground.init(this, &VDBlinn, "models/LargePlane.obj", OBJ);
		MBall.init(this, &VDBlinn, "models/Sphere.obj", OBJ);
		MskyBox.init(this, &VDskyBox, "models/SkyBoxCube.obj", OBJ);
		MGun.init(this, &VDBlinn, "models/gun.obj", OBJ);
        
        MAnimals[0].init(this, &VDBlinn, "models/animals/rhinoceros_001.mgcg", MGCG);
        MAnimals[1].init(this, &VDBlinn, "models/animals/tiger_001.mgcg", MGCG);
        MAnimals[2].init(this, &VDBlinn, "models/animals/wolf_002.mgcg", MGCG);
        MAnimals[3].init(this, &VDBlinn, "models/animals/zebra_001.mgcg", MGCG);
        MAnimals[4].init(this, &VDBlinn, "models/animals/bear_001.mgcg", MGCG);

        MAnimals[5].init(this, &VDBlinn, "models/animals/bison_001.mgcg", MGCG);
        MAnimals[6].init(this, &VDBlinn, "models/animals/bull_001.mgcg", MGCG);
        MAnimals[7].init(this, &VDBlinn, "models/animals/duck_001.mgcg", MGCG);
        MAnimals[8].init(this, &VDBlinn, "models/animals/elephant_001.mgcg", MGCG);
        MAnimals[9].init(this, &VDBlinn, "models/animals/lion_female_001.mgcg", MGCG);

        MVegRocks[0].init(this, &VDBlinn, "models/VegRocks/vegetation.001.mgcg", MGCG);
        MVegRocks[1].init(this, &VDBlinn, "models/VegRocks/vegetation.018.mgcg", MGCG);
        MVegRocks[2].init(this, &VDBlinn, "models/VegRocks/vegetation.022.mgcg", MGCG);
        MVegRocks[3].init(this, &VDBlinn, "models/VegRocks/vegetation.025.mgcg", MGCG);
        MVegRocks[4].init(this, &VDBlinn, "models/VegRocks/vegetation.044.mgcg", MGCG); //rock tree
        MVegRocks[5].init(this, &VDBlinn, "models/VegRocks/vegetation.045.mgcg", MGCG); //big rock
        MVegRocks[6].init(this, &VDBlinn, "models/VegRocks/vegetation.048.mgcg", MGCG); // rock grass
        MVegRocks[7].init(this, &VDBlinn, "models/VegRocks/vegetation.049.mgcg", MGCG); // rock grass
        MVegRocks[8].init(this, &VDBlinn, "models/VegRocks/vegetation.mgcg", MGCG); // rock grass
        MVegRocks[9].init(this, &VDBlinn, "models/VegRocks/vegetation.100.mgcg", MGCG); // rock grass
        MVegRocks[10].init(this, &VDBlinn, "models/VegRocks/vegetation.103.mgcg", MGCG); // rock grass
        MVegRocks[11].init(this, &VDBlinn, "models/VegRocks/vegetation.106.mgcg", MGCG); // rock grass

        MStructures[0].init(this, &VDBlinn, "models/structure/cottage.obj", OBJ);
        MStructures[1].init(this, &VDBlinn, "models/structure/fence.obj", OBJ);
        MStructures[2].init(this, &VDBlinn, "models/structure/tower.obj", OBJ);
        MStructures[3].init(this, &VDBlinn, "models/structure/woodhouse.obj", OBJ);

    	int mainStride = sizeof(HUDVertex);  
		// Define the 4 vertices of the quad
		std::vector<HUDVertex> quadVertices = {
		    {{-0.1f, -0.1f}, {0.0f, 0.0f}},  
		    {{ 0.1f, -0.1f}, {1.0f, 0.0f}},  
		    {{ 0.1f,  0.1f}, {1.0f, 1.0f}},  
		    {{-0.1f,  0.1f}, {0.0f, 1.0f}},
		};

		// Insert the vertices
		for (const auto& vertex : quadVertices) {
		    std::vector<unsigned char> vertexData(mainStride, 0);
		    HUDVertex* V_vertex = (HUDVertex*)(&vertexData[0]);
		    
		    V_vertex->pos = vertex.pos;
		    V_vertex->UV = vertex.UV;
		    
		    MCrosshair.vertices.insert(MCrosshair.vertices.end(), vertexData.begin(), vertexData.end());
		}

		std::vector<unsigned int> quadIndices = {0, 1, 2, 2, 3, 0};

		MCrosshair.indices.insert(MCrosshair.indices.end(), quadIndices.begin(), quadIndices.end());
		MCrosshair.initMesh(this, &VDHUD);


        // Create the textures
		Tsun.init(this, "textures/2k_sun.jpg");
		Tmoon.init(this, "textures/2k_moon.jpg");
		Tground.init(this, "textures/2k_sun.jpg");
        T1.init(this,   "textures/Textures.png");
        Tanimal.init(this, "textures/Textures-Animals.png");
        TCrosshair.init(this, "textures/cros.png");
        TGun.init(this, "textures/gun.png");
        TGrass.init(this, "textures/grass1.jpg");
        // TGrass.init(this, "textures/grass1.jpg", VK_FORMAT_R8G8B8A8_SRGB, false);
        // TGrass.createTextureSampler(
		// 					  magFilter,
		// 					  minFilter,
		// 					  addressModeU,
		// 					  addressModeV,
		// 					  mipmapMode,
		// 					  anisotropyEnable,
		// 					  maxAnisotropy,
		// 					  maxLod
		// 	);				
        
        TStructures[0].init(this, "textures/cottage_diffuse.png");
        TStructures[1].init(this, "textures/fenceDiffuse.jpg");
        TStructures[2].init(this, "textures/Tower_Col.jpg");
        TStructures[3].init(this, "textures/woodenhousediffuse.png");

		TskyBox.init(this, "textures/starmap_g4k.jpg");
		Tstars.init(this, "textures/bluecloud_up.jpg");

        // emplace_back creates an instance of an object using it's constructor and then places it into a vector of instances

		//corner rocks
        vegRocks.emplace_back(glm::vec3(70.0f, -1.0f, -70.0f), 4, glm::vec3(1.5f, 3.0f, 1.5f), 120.0f, "Top Right");
        vegRocks.emplace_back(glm::vec3(-70.0f, -3.0f, -70.0f), 4, glm::vec3(2.0f, 3.3f, 2.0f), 120.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(70.0f, -1.0f, 70.0f), 4, glm::vec3(2.0f, 2.5f, 2.0f), 120.0f, "Back Right");
        vegRocks.emplace_back(glm::vec3(-70.0f, -1.0f, 70.0f), 4, glm::vec3(2.5f, 3.5f, 2.5f), 120.0f, "Back Left");
        

        // Rocks around perimeter 

        float dist = 10.0f;
        float prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3(prev_dist + dist, 0.0f, -60.0f + randomFloat(-10.0f, 10.0f)), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler+ randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "rock");
	        prev_dist = prev_dist + dist;
        }
        
        prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3(prev_dist + dist, 0.0f, 60.0f + randomFloat(-10.0f, 10.0f)), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler+ randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "rock");
	        prev_dist = prev_dist + dist;
        }

        prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3( 60.0f + randomFloat(-10.0f, 10.0f) , 0.0f, prev_dist + dist), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler+ randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "rock");
	        prev_dist = prev_dist + dist;
        }

        prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3( -60.0f + randomFloat(-10.0f, 10.0f) , 0.0f, prev_dist + dist), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler + randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "rock");
	        prev_dist = prev_dist + dist;
        }

        // TREES


        vegRocks.emplace_back(glm::vec3(40.0f, 0.0f, -32.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Right");

        vegRocks.emplace_back(glm::vec3(20.0f, 0.0f, -29.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Right");
        vegRocks.emplace_back(glm::vec3(10.0f, 0.0f, -30.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Right");
        vegRocks.emplace_back(glm::vec3(-5.0f, 0.0f, -38.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Right");
        vegRocks.emplace_back(glm::vec3(-22.0f, 0.0f, -25.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Right");

        vegRocks.emplace_back(glm::vec3(-43.0f, 0.0f, -40.0f), 1,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");

        vegRocks.emplace_back(glm::vec3(-33.0f, 0.0f, -30.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(-23.0f, 0.0f, -10.0f), 1,glm::vec3(1.5f, 2.0f, 1.5f), 0.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(-40.0f, 0.0f, 	6.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(-30.0f, 0.0f, 14.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");

        vegRocks.emplace_back(glm::vec3(-38.0f, 0.0f, 40.0f), 1,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Back Left");

        vegRocks.emplace_back(glm::vec3(-18.0f, 0.0f, 40.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Back Left");
        vegRocks.emplace_back(glm::vec3( 0.0f, 0.0f, 20.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Back Left");
        vegRocks.emplace_back(glm::vec3( 8.0f, 0.0f, 33.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Back Left");
        vegRocks.emplace_back(glm::vec3( 22.0f, 0.0f, 41.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Back Left");

        vegRocks.emplace_back(glm::vec3(40.0f, 0.0f, 20.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 90.0f, "Back Right");

        vegRocks.emplace_back(glm::vec3(30.0f, 0.0f, -30.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(43.0f, 0.0f, -10.0f), 1,glm::vec3(1.5f, 2.0f, 1.5f), 0.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(23.0f, 0.0f, 	6.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");
        vegRocks.emplace_back(glm::vec3(43.0f, 0.0f, 14.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Top Left");

		vegRocks.emplace_back(glm::vec3(5.0f, 0.0f, -5.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Test Tree");


        /// STRUCTURES



		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(-40.0f +  2.5f*i, 0.0f, -36.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence1");
		}
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(-40.0f +  2.5f*i, 0.0f, 45.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence1"); //direction of spawning
		}
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(43.0, 0.0f, -36.0f +  2.5f*i), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence2");
			structures[68+i].angle2 = 90.0f;
		}	
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(-40.0, 0.0f, -36.0f +  2.5f*i), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence2");
			structures[102+i].angle2 = 90.0f;
		}		

        structures.emplace_back(glm::vec3(0.0f, 0.0f, 10.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Cottage");
        structures.emplace_back(glm::vec3(10.0f, 0.0f, -10.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "House");
        structures.emplace_back(glm::vec3(4.0f, 0.0f, -10.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence");
        structures.emplace_back(glm::vec3(5.5f, 0.0f, -10.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence");
        structures.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "TOWER");

        // ANIMALS

		glm::vec3 modelMin(-1.0f, -0.5f, -1.0f);
		glm::vec3 modelMax(  1.0f, 1.5f, 1.0f); 
        for(int i = 0; i < NANIMAL; i ++){
	        animals.emplace_back(glm::vec3(0.0f + i*2.0f, 1.0f, 0.0f), i, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "animal");
	        animals[i].updateBoundingBox(modelMin, modelMax);
	        createBoundingBoxModel(MBBox, animals[i].bbox, VDBBox);
        }

        // balls for visualiing ray
        for(int i = 0; i < NBALLS; i ++){
	        balls.emplace_back(glm::vec3(0.0f + i, 0.0f, 0.0f), i, glm::vec3(0.1f, 0.1f, 0.1f), 0.0f, "ball");
        }







		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene
		// Update the number of elements to correctly size the descriptor sets pool
		DPSZs.uniformBlocksInPool = 500;
		DPSZs.texturesInPool = 400;
		DPSZs.setsInPool = 400;

		std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
		
		ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PBlinn.create();
		PEmission.create();
		PskyBox.create();
		PHUD.create();
		PBBox.create();
        
		// Here you define the data set
		DSsun.init(this, &DSLEmission, {&Tsun});
		DSmoon.init(this, &DSLEmission, {&Tmoon});
		DSground.init(this, &DSLBlinn, {&TGrass});
        DSAx.init(this, &DSLBlinn, {&T1});
        DSCrosshair.init(this, &DSLHUD, {&TCrosshair});
        DSBBox.init(this, &DSLBBox, {&T1});
		DSskyBox.init(this, &DSLskyBox, {&TskyBox, &Tstars});
		DSGun.init(this, &DSLBlinn, {&TGun});


        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.init(this, &DSLBlinn, {&Tanimal});
        };

        for (DescriptorSet &DSVegRock: DSVegRocks) {
            DSVegRock.init(this, &DSLBlinn, {&T1});
        };

        for (int index = 0; index < NSTRUCTURES; index++) {
        	const Instance& instance = structures[index];
            DSStructures[index].init(this, &DSLBlinn, {&TStructures[instance.modelID]});
        }

        for (DescriptorSet &DSBall: DSBalls) {
            DSBall.init(this, &DSLBlinn, {&T1});
        };
			
		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		

		std::cout << "Descriptor Set init!\n";

	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PBlinn.cleanup();
		PEmission.cleanup();
		PHUD.cleanup();
		PBBox.cleanup();
		PskyBox.cleanup();

		DSsun.cleanup();
		DSmoon.cleanup();
		DSGlobal.cleanup();
		DSground.cleanup();
        DSAx.cleanup();
        DSCrosshair.cleanup();
        DSBBox.cleanup();
		DSskyBox.cleanup();
		DSGun.cleanup();


        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.cleanup();
        }
        for (DescriptorSet &DSVegRock: DSVegRocks) {
            DSVegRock.cleanup();
        }        
        for (DescriptorSet &DSStructure: DSStructures) {
            DSStructure.cleanup();
        };
        for (DescriptorSet &DSBall: DSBalls) {
            DSBall.cleanup();
        };
		txt.pipelinesAndDescriptorSetsCleanup();

        std::cout << "Descriptor Set cleanup!\n";

	}


	void localCleanup() {	

		Tsun.cleanup();
		Tmoon.cleanup();
		Tground.cleanup();
        T1.cleanup();
        Tanimal.cleanup();
        TCrosshair.cleanup();
        TGun.cleanup();	

        for (Texture &Tstruct: TStructures) {
            Tstruct.cleanup();
        }

		MCrosshair.cleanup();
		MBBox.cleanup();
		Msun.cleanup();
		Mmoon.cleanup();
		Mground.cleanup();
        MAx.cleanup();
        MBall.cleanup();
        MGun.cleanup();
        for (Model &MAnimal: MAnimals) {
            MAnimal.cleanup();
        }
        for (Model &MVegRock: MVegRocks) {
            MVegRock.cleanup();
        }
        for (Model &MStructure: MStructures) {
            MStructure.cleanup();
        }

		TskyBox.cleanup();
		Tstars.cleanup();
		MskyBox.cleanup();

		// Cleanup descriptor set layouts
		DSLBlinn.cleanup();
		DSLEmission.cleanup();
		DSLGlobal.cleanup();
		DSLHUD.cleanup();
		DSLBBox.cleanup();
		DSLskyBox.cleanup();
		
		// Destroies the pipelines
		PBlinn.destroy();
		PEmission.destroy();
		PHUD.destroy();
		PBBox.destroy();
		PskyBox.destroy();

		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		PBlinn.bind(commandBuffer);
		// The models (both index and vertex buffers)

		// The descriptor sets, for each descriptor set specified in the pipeline
		DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);	// The Global Descriptor Set (Set 0)

		for (int index = 0; index < NANIMAL; index++) {
		    const Instance& instance = animals[index];
            MAnimals[instance.modelID].bind(commandBuffer);
            DSAnimals[index].bind(commandBuffer, PBlinn, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MAnimals[instance.modelID].indices.size()), 1, 0, 0, 0);
        }

		for (int index = 0; index < NVEGROCK; index++) {
		    const Instance& instance = vegRocks[index];
            MVegRocks[instance.modelID].bind(commandBuffer);
            DSVegRocks[index].bind(commandBuffer, PBlinn, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MVegRocks[instance.modelID].indices.size()), 1, 0, 0, 0);
        }

		for (int index = 0; index < NSTRUCTURES; index++) {
		    const Instance& instance = structures[index];
            MStructures[instance.modelID].bind(commandBuffer);
            DSStructures[index].bind(commandBuffer, PBlinn, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MStructures[instance.modelID].indices.size()), 1, 0, 0, 0);
        }
		for (int index = 0; index < NBALLS; index++) {
		    const Instance& instance = balls[index];
            MBall.bind(commandBuffer);
            DSBalls[index].bind(commandBuffer, PBlinn, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MBall.indices.size()), 1, 0, 0, 0);
        }

		Mground.bind(commandBuffer);
		DSground.bind(commandBuffer, PBlinn, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mground.indices.size()), 1, 0, 0, 0);	

        MAx.bind(commandBuffer);
        DSAx.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MAx.indices.size()), 1, 0, 0, 0);

		MGun.bind(commandBuffer);
		DSGun.bind(commandBuffer, PBlinn, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MGun.indices.size()), 1, 0, 0, 0);	

		PEmission.bind(commandBuffer);
		Msun.bind(commandBuffer);
		DSsun.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);	

		Mmoon.bind(commandBuffer);
		DSmoon.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mmoon.indices.size()), 1, 0, 0, 0);

		PskyBox.bind(commandBuffer);
		MskyBox.bind(commandBuffer);
		DSskyBox.bind(commandBuffer, PskyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MskyBox.indices.size()), 1, 0, 0, 0);

		PBBox.bind(commandBuffer);
		MBBox.bind(commandBuffer);
		DSBBox.bind(commandBuffer, PBBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MBBox.indices.size()), 1, 0, 0, 0);	

		PHUD.bind(commandBuffer);
		MCrosshair.bind(commandBuffer);
		DSCrosshair.bind(commandBuffer, PHUD, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCrosshair.indices.size()), 1, 0, 0, 0);	

            
		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		
		static float autoTime = true;
		static float cTime = 0.0;
		float turnTime = 72.0f * 10.0f;
		if (isDebugMode)
			turnTime = 72.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		
		if(autoTime) {
			cTime = cTime + deltaT;
			cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
		}

		static float tTime = 0.0;
		const float TturnTime = 60.0f;
		const float TangTurnTimeFact = 1.0f / TturnTime;
		
		if(autoTime) {
			tTime = tTime + deltaT;
			tTime = (tTime > TturnTime) ? (tTime - TturnTime) : tTime;
		}

		float dayTime = sin(cTime * angTurnTimeFact);
		
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 10.0f;
		
		static float CamPitch = glm::radians(0.0f);
		static float CamYaw   = M_PI;

		if (currScene == MATCH) {
			if (!isDebugMode)
			{
				glm::vec3 FirstPos = glm::vec3(2, 1, 5);
				glm::vec3 Pos = glm::vec3(0, 0, 0);

				glm::vec3 player_position = Pos + FirstPos;

				float Yaw = 0;

				// static float CamPitch = glm::radians(0.0f);
				// static float CamYaw   = M_PI;
				static float CamRoll  = 0.0f;

				static float dampedVelz = 0.0f;
				static float dampedVelx = 0.0f;

				const float WALKING_SPEED = 2.0f;

				static glm::vec3 lastPos;

				dampedVelz =  WALKING_SPEED * deltaT * m.z + dampedVelz;
				dampedVelx =  WALKING_SPEED * deltaT * m.x + dampedVelx;

				dampedVelz = dampedVelz * 0.85f;
				dampedVelx = dampedVelx * 0.85f;

				glm::vec3 ux = glm::vec3(cos(CamYaw), 0.0f, -sin(CamYaw));
				glm::vec3 uz = glm::vec3(sin(CamYaw), 0.0f, cos(CamYaw));

				CamYaw -= ROT_SPEED * deltaT * r.y;
				CamPitch -= ROT_SPEED * deltaT * r.x;
				
				Pos = lastPos + ux * dampedVelx + uz * dampedVelz;

				if(Pos.x > 40.0f){
					std::cout << "Pos x too big "<< Pos.x << "\n";
					Pos = glm::vec3( 40.0f, Pos.y, Pos.z);
				}
				if(Pos.x < -40.0f){
					std::cout << "Pos x too small "<< Pos.x << "\n";
					Pos = glm::vec3( -40.0f, Pos.y, Pos.z);
				}
				if(Pos.z > 40.0f){
					std::cout << "Pos z too big "<< Pos.z << "\n";
					Pos = glm::vec3( Pos.x, Pos.y, 40.0f);
				}
				if(Pos.z < -40.0f){
					std::cout << "Pos z too small "<< Pos.z << "\n";
					Pos = glm::vec3( Pos.x, Pos.y, -40.0f);
				}

				//glm::vec3 new_position = lastPos + ux * dampedVelx + uz * dampedVelz;

				bool collision_detected = false;
        		for (int index = 0; index < NVEGROCK; index++) {
					float objRadius = 1.0f;
					if(vegRocks[index].modelID < 4){
				
						if (checkCollision(Pos + FirstPos, 0.5f, vegRocks[index].pos, 1.0f)) {
							std::cout << "collided" << "\n";
							std::cout << vegRocks[index].scale.x << "\n";
							std::cout << vegRocks[index].desc << "\n";
							glm::vec3 collision_direction = glm::normalize( (Pos + FirstPos) - vegRocks[index].pos);
							collision_direction = glm::vec3(collision_direction.x, 0.0f, collision_direction.z);
							float pushback_distance = (2.5f) - glm::length((Pos + FirstPos) - vegRocks[index].pos);
							Pos = Pos + pushback_distance * collision_direction;
						}
					}
				}

			
				CamPitch = (CamPitch < -0.25*M_PI ? -0.25*M_PI : (CamPitch > 0.25*M_PI ? 0.25*M_PI : CamPitch));

				lastPos = Pos;

				ViewMatrix = 
				glm::rotate(glm::mat4(1.0), -CamPitch, glm::vec3(1,0,0)) *
					glm::rotate(glm::mat4(1.0), -CamYaw, glm::vec3(0,1,0)) *
					glm::translate(glm::mat4(1.0), -(Pos + FirstPos)) * glm::mat4(1.0);

			} else {
				// The Fly model update proc.
				ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT,
									glm::vec3(1, 0, 0)) * ViewMatrix;
				ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.y * deltaT,
									glm::vec3(0, 1, 0)) * ViewMatrix;
				ViewMatrix = glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT,
									glm::vec3(0, 0, 1)) * ViewMatrix;
				ViewMatrix = glm::translate(glm::mat4(1), -glm::vec3(
									MOVE_SPEED * m.x * deltaT, MOVE_SPEED * m.y * deltaT, MOVE_SPEED * m.z * deltaT))
														* ViewMatrix;
			}
			PlayerPos = extractPlayerPositionFromViewMatrix(ViewMatrix);
		}
		static float subpassTimer = 0.0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
								
				// Shoots only during the match
				if (currScene == MATCH) {

					//RebuildPipeline();
					// std::cout << "New Ray Direction: " << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << std::endl;
					std::cout << "player pos " << PlayerPos.x << ", " << PlayerPos.y << ", " << PlayerPos.z << std::endl;

					//std::cout << "animal: " << animals[0].pos.x << ", " << animals[0].pos.y << ", " << animals[0].pos.z << std::endl;

					float t0, t1;
					for (int index = 0; index < NANIMAL; index++) {
						if(animals[index].visible){
							if(rayIntersectsSphere(ray.origin, ray.direction, animals[index].pos, 0.5, t0, t1)){ 
								std::cout<< "Sphere HIT" << std::endl;
								animals[index].visible = false;
								RebuildPipeline();
							}
						}
					}
				}
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_V)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printMat4("ViewMatrix  ", ViewMatrix);				
				std::cout << "cTime    = " << cTime    << ";\n";
				std::cout << "tTime    = " << tTime    << ";\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_ENTER)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_ENTER;

				if (currScene == STARTMENU || currScene == MATCH)
					currScene++;
				else if (currScene == GAMEOVER)
					currScene = MATCH;
				else
					std::cout << "Error, wrong scene : " << currScene << "\n";

				std::cout << "Scene : " << currScene << "\n";
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_ENTER) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		}


		// Here is where you actually update your uniforms
		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;
		glm::mat4 ViewPrj =  M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);		

		ray = calculateRayFromScreenCenter(Mv, M);						

		// updates global uniforms
		// Global
		GlobalUniformBufferObject gubo{};
		// gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact));
		gubo.lightDir = glm::vec3(cos(cTime * angTurnTimeFact), dayTime, 0.0f);

		float intensity = glm::min(0.0f, dayTime);
		float maxDegree = 0.342f;
		float dayIntensity = dayTime;
		float nightIntensity = -dayTime;

		if (dayIntensity > 0.0f) {
			dayIntensity = 1.0f;
		} else if (dayIntensity < -maxDegree) {
			dayIntensity = 0.0f;
		} else {
			dayIntensity = (dayIntensity + maxDegree) / maxDegree;
		}

		if (nightIntensity > maxDegree) {
			nightIntensity = 1.0f;
		} else if (nightIntensity < -maxDegree) {
			nightIntensity = 0.0f;
		} else {
			nightIntensity = (nightIntensity + maxDegree) / (2*maxDegree);
		}
		
		gubo.dayLightColor = glm::vec4(
			1.0f * dayIntensity, 
			(0.2f + 0.8f * abs(dayTime)) * dayIntensity, 
			abs(dayTime * dayIntensity),
			1.0f
			);
		
		gubo.nightLightColor = glm::vec4(
			0.1f * nightIntensity, 
			0.4f * nightIntensity, 
			0.4f * nightIntensity, 
			1.0f
			);
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		gubo.ambient = dayTime;
		DSGlobal.map(currentImage, &gubo, 0);

		// objects
		BlinnUniformBufferObject blinnUbo{};
		BlinnMatParUniformBufferObject blinnMatParUbo{};

		// BlinnUniformBufferObject bboxUbo{};
        // bboxUbo.mMat = glm::mat4(1);
        // bboxUbo.nMat = glm::mat4(1);
        // bboxUbo.mvpMat = ViewPrj;
    	//DSBBox.map(currentImage, &bboxUbo, 0);

		EmissionUniformBufferObject emissionUbo{};
		EmissionParUniformBufferObject epubo{};

		emissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), gubo.lightDir * 100.0f) * glm::scale(glm::mat4(1), glm::vec3(5.0f)) * baseTr;
		epubo.temperature = dayTime;

		DSsun.map(currentImage, &emissionUbo, 0);
		DSsun.map(currentImage, &epubo, 2);

		EmissionUniformBufferObject moonEmissionUbo{};
		EmissionParUniformBufferObject moonepubo{};
		moonEmissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), -gubo.lightDir * 100.0f) * glm::scale(glm::mat4(1), glm::vec3(4.0f))  * baseTr;
		moonepubo.temperature = -2.0f;
		DSmoon.map(currentImage, &moonEmissionUbo, 0);
		DSmoon.map(currentImage, &moonepubo, 2);

		SkyBoxUniformBufferObject sbubo{};
		sbubo.mvpMat = M * glm::mat4(glm::mat3(Mv));
		DSskyBox.map(currentImage, &sbubo, 0);

		SkyBoxParUniformBufferObject dtubo{};
		dtubo.daytime = dayTime;
		DSskyBox.map(currentImage, &dtubo, 3);
		           
        blinnUbo.mMat = glm::mat4(1);
        blinnUbo.nMat = glm::mat4(1);
        blinnUbo.mvpMat = ViewPrj;
        blinnMatParUbo.Power = 200.0;
		blinnMatParUbo.scaleUV = 1.0;

        // AXIS
        DSAx.map(currentImage, &blinnUbo, 0); 
        DSAx.map(currentImage, &blinnMatParUbo, 2);

        // GROUND
        blinnUbo.mMat = glm::translate(glm::scale(glm::mat4(1), glm::vec3(4,1,4)),
                                           glm::vec3( 0 , 0, 0)) 
 											* glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f)) * baseTr;
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
       	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

        DSground.map(currentImage, &blinnUbo, 0); //DSground
        blinnMatParUbo.scaleUV = 50.0;
		blinnMatParUbo.Power = 200000.0;
        DSground.map(currentImage, &blinnMatParUbo, 2);

        // GUN
		glm::mat4 gunModelMatrix = 
 								  glm::translate(glm::mat4(1.0f), PlayerPos) 
								* glm::rotate(glm::mat4(1.0f), (CamYaw), glm::vec3(0.0f, 1.0f, 0.0f))
								* glm::rotate(glm::mat4(1.0f), (CamPitch + glm::radians(10.0f)), glm::vec3(1.0f, 0.0f, 0.0f))
								* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),glm::vec3(0.0f, 1.0f, 0.0f))
								* glm::translate(glm::mat4(1.0f), glm::vec3(0.45f, -0.25f, 0.0f));


		blinnUbo.mMat = gunModelMatrix;
    	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
    	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
        DSGun.map(currentImage, &blinnUbo, 0);

		blinnMatParUbo.Power = 2000.0;
		blinnMatParUbo.scaleUV = 1.0;
    	DSGun.map(currentImage, &blinnMatParUbo, 2);

        // ANIMALS
        for (int model = 0; model < NANIMAL; model++) {
        	const Instance& instance = animals[model];
        	if(instance.visible == true)
        	{
	 			blinnUbo.mMat = glm::translate(glm::mat4(1.0f),
	                                           instance.pos) 
	 											* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f))
	 											* glm::rotate(glm::scale(glm::mat4(1), glm::vec3(0.5,0.5,0.5)), glm::radians(instance.angle),glm::vec3(0.0f, 0.0f, 1.0f)) * baseTr;
	        	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
	        	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
	            DSAnimals[model].map(currentImage, &blinnUbo, 0);

				blinnMatParUbo.Power = 2000.0;
	        	DSAnimals[model].map(currentImage, &blinnMatParUbo, 2);
        	}
        }

        // VEG ROCKs
        for (int model = 0; model < NVEGROCK; model++) {
        	const Instance& instance = vegRocks[model];

 			blinnUbo.mMat = glm::translate(glm::mat4(1.0f),
                                           instance.pos) 
 											* glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f))
 											* glm::rotate(glm::scale(glm::mat4(1), instance.scale), glm::radians(instance.angle),glm::vec3(0.0f, 1.0f, 0.0f)) * baseTr;


        	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

            DSVegRocks[model].map(currentImage, &blinnUbo, 0);

			blinnMatParUbo.Power = 2000.0;
        	DSVegRocks[model].map(currentImage, &blinnMatParUbo, 2);
        }

        // STRUCUTRES
        for (int model = 0; model < NSTRUCTURES; model++) {
        	const Instance& instance = structures[model];

 			blinnUbo.mMat = glm::translate(glm::mat4(1.0f),
                                           instance.pos) 
 											* glm::rotate(glm::mat4(1.0f), glm::radians(instance.angle2),glm::vec3(0.0f, 1.0f, 0.0f))
 											* glm::rotate(glm::scale(glm::mat4(1), instance.scale), glm::radians(instance.angle),glm::vec3(1.0f, 0.0f, 0.0f)) * baseTr;


        	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

            DSStructures[model].map(currentImage, &blinnUbo, 0);

			blinnMatParUbo.Power = 200.0;
        	DSStructures[model].map(currentImage, &blinnMatParUbo, 2);
        }

		if (isDebugMode){

	        // debugging balls glm::vec3 rayEnd = rayOrigin + rayDirection * 2.0f;
	        for (int model = 0; model < NBALLS; model++) {
	        	const Instance& instance = balls[model];
	 			blinnUbo.mMat = glm::translate(glm::mat4(1.0f),
	                                           (ray.origin + ray.direction * (10.0f + 20.0f*sin(cTime)))) 
	 											* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f))
	 											* glm::rotate(glm::scale(glm::mat4(1), glm::vec3(0.1,0.1,0.1)), glm::radians(instance.angle),glm::vec3(0.0f, 0.0f, 1.0f)) * baseTr;
	        	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
	        	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));
	            DSBalls[model].map(currentImage, &blinnUbo, 0);
				blinnMatParUbo.Power = 2000.0;
	        	DSBalls[model].map(currentImage, &blinnMatParUbo, 2);
	        }
    	}
	}
};



// This is the main: probably you do not need to touch this!
int main(int argc, char* argv[]) {

    if (argc > 1) { // Check if any arguments are provided
        std::string arg = argv[1];
        if (arg == "-debug") {
            isDebugMode = true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }
    if (isDebugMode) {
        std::cout << "Running in Debug mode" << std::endl;
        // Debug-specific code here
    } else {
        std::cout << "Running in Release mode" << std::endl;
        // Release-specific code here
    }

    HuntGame app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
