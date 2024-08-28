// This has been adapted from the Vulkan tutorial
// And from A10

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include <cstdlib> 

bool isDebugMode = false;

std::vector<SingleText> outText = {
	{2, {"Adding an object", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
};

// THE NUMBER OF INSTANCES OF ANIMALS, VEGITATION/ROCKS, STRUCTURES
#define NANIMAL 10 
#define NVEGROCK 72
#define NSTRUCTURES 5

struct BlinnUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct BlinnMatParUniformBufferObject {
	alignas(4)  float Power;
};

struct EmissionUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

// The vertices data structures
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


// class used to create instances of an object

class Instance {
public:
    glm::vec3 pos;  // instance position
    int modelID;    // model id used to get the model of the object     
    glm::vec3 scale; // scale    
    float angle;     // angle can be used to roate object    
    std::string desc; // description of the instance 

    // Constructor
    Instance(const glm::vec3& position, int id, const glm::vec3& scl, float ang, const std::string& description)
        : pos(position), modelID(id), scale(scl), angle(ang), desc(description) {}
};


float randomFloat(float min, float max) {
    // Generate a random float between 0 and 1
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    // Scale and shift to the desired range [min, max]
    return min + random * (max - min);
}



// MAIN ! 
class HuntGame : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal;	    // For Global values
	DescriptorSetLayout DSLBlinn;	    // For Blinn Objects
	DescriptorSetLayout DSLEmission;	// For Emission Objects
	DescriptorSetLayout DSLHUD;

	// Vertex formats
	VertexDescriptor VDBlinn;
	VertexDescriptor VDEmission;
	VertexDescriptor VDHUD;

	// Pipelines [Shader couples]
	Pipeline PBlinn;
	Pipeline PEmission;
	Pipeline PHUD;

	// Scenes and texts
    TextMaker txt;
    
    // Models
    Model MAx; // xyz axis object
    Model Msun;
    Model Mground;

	// we have 12 different models of different vegitiation and rocks, but we can have many more instances of these models
    Model MVegRocks[12];
    std::vector<Instance> vegRocks; // vector holding all instances of the vegrocks

    Model MStructures[4];
    std::vector<Instance> structures;

    Model MAnimals[10];
    std::vector<Instance> animals;

    //HUD
    // Vertex data for a centered quad
	HUDVertex HUDvertices[4] = {
    	{{-0.05f, -0.05f}, {0.0f, 0.0f}},  // Bottom-left
    	{{ 0.05f, -0.05f}, {1.0f, 0.0f}},  // Bottom-right
    	{{ 0.05f,  0.05f}, {1.0f, 1.0f}},  // Top-right
    	{{-0.05f,  0.05f}, {0.0f, 1.0f}},  // Top-left
	};
	//HUDVertex HUDvertices[4];

	uint16_t HUDindices[6] = {0, 1, 2, 2, 3, 0};

    
    // Descriptor Sets
    DescriptorSet DSGlobal, DSAx, DSCrosshair;
    DescriptorSet DSsun;
    DescriptorSet DSground;
    DescriptorSet DSAnimals[NANIMAL];
    DescriptorSet DSVegRocks[NVEGROCK];
    DescriptorSet DSStructures[NSTRUCTURES];


    
   // Textures
    Texture T1, Tanimal;
    Texture Tsun;
    Texture Tground;
    Texture TStructures[4];
    Texture TCrosshair;
    
	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
	glm::mat4 ViewMatrix;

	float Ar;
	
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
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
				});
		DSLHUD.init(this, {
    			{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}  // Crosshairs texture sampler
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

		VDHUD.init(this, {
				  {0, sizeof(HUDVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(HUDVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(HUDVertex, UV),
				         sizeof(glm::vec2), UV}
				});

//  Place here the initialization for the VertexDescriptor

              
		// Pipelines [Shader couples]
		PBlinn.init(this, &VDBlinn,  "shaders/BlinnVert.spv",    "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
		PEmission.init(this, &VDEmission,  "shaders/EmissionVert.spv",    "shaders/EmissionFrag.spv", {&DSLEmission});
		PHUD.init(this, &VDHUD, "shaders/HUDVert.spv", "shaders/HUDFrag.spv", {&DSLHUD});
        PHUD.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);
 
		// Create models
        MAx.init(this, &VDBlinn, "models/axis.obj", OBJ);
		Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		Mground.init(this, &VDBlinn, "models/LargePlane.obj", OBJ);
        
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




        // Create the textures
		Tsun.init(this, "textures/2k_sun.jpg");
		Tground.init(this, "textures/2k_sun.jpg");
        T1.init(this,   "textures/Textures.png");
        Tanimal.init(this, "textures/Textures-Animals.png");
        TCrosshair.init(this, "textures/crosshair.png");
        
        TStructures[0].init(this, "textures/cottage_diffuse.png");
        TStructures[1].init(this, "textures/fenceDiffuse.jpg");
        TStructures[2].init(this, "textures/Tower_Col.jpg");
        TStructures[3].init(this, "textures/woodenhousediffuse.png");

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
	        vegRocks.emplace_back(glm::vec3(prev_dist + dist, 0.0f, -60.0f + randomFloat(-10.0f, 10.0f)), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler+ randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "Top Right");
	        prev_dist = prev_dist + dist;
        }
        
        prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3(prev_dist + dist, 0.0f, 60.0f + randomFloat(-10.0f, 10.0f)), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler+ randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "Top Right");
	        prev_dist = prev_dist + dist;
        }

        prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3( 60.0f + randomFloat(-10.0f, 10.0f) , 0.0f, prev_dist + dist), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler+ randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "Top Right");
	        prev_dist = prev_dist + dist;
        }

        prev_dist = -60.0f;
        for(int i = 0; i < 12; i++){
        	float scaler = 5.0f + randomFloat(-1.0f, 2.0f);
	        vegRocks.emplace_back(glm::vec3( -60.0f + randomFloat(-10.0f, 10.0f) , 0.0f, prev_dist + dist), (int)randomFloat(8.0f, 11.0f), glm::vec3(scaler, scaler + randomFloat(0.0f, 2.0f), scaler), randomFloat(0.0f, 360.0f), "Top Right");
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

        /// STRUCTURES

        structures.emplace_back(glm::vec3(0.0f, 0.0f, 10.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Cottage");
        structures.emplace_back(glm::vec3(10.0f, 0.0f, -10.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "House");
        structures.emplace_back(glm::vec3(4.0f, 0.0f, -10.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence");
        structures.emplace_back(glm::vec3(5.5f, 0.0f, -10.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence");
        structures.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "TOWER");


        // ANIMALS

        for(int i = 0; i < NANIMAL; i ++){
	        animals.emplace_back(glm::vec3(1.0f + i, 0.0f, 0.0f), i, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "animal");
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
		PHUD.create();
        
		// Here you define the data set
		DSsun.init(this, &DSLEmission, {&Tsun});
		DSground.init(this, &DSLBlinn, {&T1});
        DSAx.init(this, &DSLBlinn, {&T1});
        DSCrosshair.init(this, &DSLHUD, {&TCrosshair});

        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.init(this, &DSLBlinn, {&Tanimal});
            std::cout << "DSAnimal.init!\n";
        };

        for (DescriptorSet &DSVegRock: DSVegRocks) {
            DSVegRock.init(this, &DSLBlinn, {&T1});
            std::cout << "DSVegRocks.init!\n";
        };

        for (int index = 0; index < NSTRUCTURES; index++) {
        	const Instance& instance = structures[index];
            DSStructures[index].init(this, &DSLBlinn, {&TStructures[instance.modelID]});
        }
			
		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PBlinn.cleanup();
		PEmission.cleanup();
		PHUD.cleanup();

		DSsun.cleanup();
		DSGlobal.cleanup();
		DSground.cleanup();
        DSAx.cleanup();
        DSCrosshair.cleanup();

        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.cleanup();
            std::cout << "DSAnimal.cleanup!\n";
        }
        for (DescriptorSet &DSVegRock: DSVegRocks) {
            DSVegRock.cleanup();
            std::cout << "DSVegRock.cleanup!\n";
        }        

        for (DescriptorSet &DSStructure: DSStructures) {
            DSStructure.cleanup();
            std::cout << "DSStructure.init!\n";
        };

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	

		Tsun.cleanup();
		Tground.cleanup();
        T1.cleanup();
        Tanimal.cleanup();
        TCrosshair.cleanup();

        for (Texture &Tstruct: TStructures) {
            Tstruct.cleanup();
        }

		Msun.cleanup();
		Mground.cleanup();
        MAx.cleanup();
        for (Model &MAnimal: MAnimals) {
            MAnimal.cleanup();
        }
        for (Model &MVegRock: MVegRocks) {
            MVegRock.cleanup();
        }
        for (Model &MStructure: MStructures) {
            MStructure.cleanup();
        }


		
		// Cleanup descriptor set layouts
		DSLBlinn.cleanup();
		DSLEmission.cleanup();
		DSLGlobal.cleanup();
		DSLHUD.cleanup();
		
		// Destroies the pipelines
		PBlinn.destroy();
		PEmission.destroy();
		PHUD.destroy();

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

		Mground.bind(commandBuffer);
		DSground.bind(commandBuffer, PBlinn, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mground.indices.size()), 1, 0, 0, 0);	

        MAx.bind(commandBuffer);
        DSAx.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MAx.indices.size()), 1, 0, 0, 0);




        
		PEmission.bind(commandBuffer);
		Msun.bind(commandBuffer);
		DSsun.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);	


		PHUD.bind(commandBuffer);

            
		//txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
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
		const float turnTime = 72.0f;
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
		
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 10.0f;
		
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
		static float subpassTimer = 0.0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				//RebuildPipeline();
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

		if(glfwGetKey(window, GLFW_KEY_C)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_C;
				}
		} else {
			if((curDebounce == GLFW_KEY_C) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(glfwGetKey(window, GLFW_KEY_T)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_T;
			}
		} else {
			if((curDebounce == GLFW_KEY_T) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

					//ViewMatrix = glm::translate(glm::mat4(1), -CamPos);

		// Here is where you actually update your uniforms
		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;

		glm::mat4 ViewPrj =  M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);								

		// updates global uniforms
		// Global
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		DSGlobal.map(currentImage, &gubo, 0);

		// objects
		BlinnUniformBufferObject blinnUbo{};
		BlinnMatParUniformBufferObject blinnMatParUbo{};

        
        int i = 0;
        int j = 2;
        int k = 2;




		EmissionUniformBufferObject emissionUbo{};
		emissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), gubo.lightDir * 40.0f) * baseTr;
		DSsun.map(currentImage, &emissionUbo, 0);
		
		
            
        blinnUbo.mMat = glm::mat4(1);
        blinnUbo.nMat = glm::mat4(1);
        blinnUbo.mvpMat = ViewPrj;
        blinnMatParUbo.Power = 200.0;

        DSAx.map(currentImage, &blinnUbo, 0); 
        DSAx.map(currentImage, &blinnMatParUbo, 2);


        blinnUbo.mMat = glm::translate(glm::scale(glm::mat4(1), glm::vec3(4,1,4)),
                                           glm::vec3( 0 , 0, 0)) 
 											* glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f)) * baseTr;
        blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
       	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

        DSground.map(currentImage, &blinnUbo, 0); //DSground
        DSground.map(currentImage, &blinnMatParUbo, 2);

        //ANIMALS
        for (int model = 0; model < NANIMAL; model++) {
        	const Instance& instance = animals[model];

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
 											* glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f))
 											* glm::rotate(glm::scale(glm::mat4(1), instance.scale), glm::radians(instance.angle),glm::vec3(1.0f, 0.0f, 0.0f)) * baseTr;


        	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

            DSStructures[model].map(currentImage, &blinnUbo, 0);

			blinnMatParUbo.Power = 2000.0;
        	DSStructures[model].map(currentImage, &blinnMatParUbo, 2);
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
