#include "hunt.h"

// MAIN ! 
class HuntGame : public BaseProject {
	private:
	Ray ray;

	DescriptorSetLayout DSLGlobal, DSLBlinn, DSLOren, DSLEmission, DSLHUD, DSLskyBox, DSLNMap;
	VertexDescriptor VDBlinn, VDOren, VDEmission, VDHUD, VDskyBox, VDNMap;
	Pipeline PBlinn, POren, PEmission, PHUD, PskyBox, PNMAp;

	// Texts
    TextMaker txt, timeTxt, animalTxt;
    
    // Models
    Model MAx,Msun, Mmoon, Mground, MCrosshair, MScope, MskyBox, MMenuScreen, MGun, MBall, MTower;
	Model MVegRocks[12], MStructures[4], MAnimals[10];

	glm::vec3 towerPos = glm::vec3(-23.0f, 0.0f, 24.0f);

    // Descriptor Sets
	DescriptorSet DSGlobal, DSAx, DSCrosshair, DSScope, DSsun, DSmoon, DSground, DSskyBox, DSGun, DSMenuScreen, DSTower;
	DescriptorSet DSAnimals[NANIMAL], DSVegRocks[NVEGROCK], DSStructures[NSTRUCTURES], DSBalls[NBALLS];

   // Textures
	Texture T1, Tanimal, TGun, TGrass, Tsun, Tmoon, Tground, TCrosshair, TskyBox, Tstars, TMenuScreen, TScope;
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

	// Other application parameters
	int currScene = STARTMENU;
	int aliveAnimals = NANIMAL;
	float startTime = 0.0f;
	float currTime = 0.0f;
	float lastTime = 0.0f;
	int currTimeIndex = 0;
	int currAnimalIndex = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 0.1, 5.0);
	glm::vec3 PlayerPos = glm::vec3(0.0, 0.1, 5.0);
	glm::mat4 ViewMatrix;
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;	

	float Ar;
	float cameraZoom = 50.0f;
	float zoomInAngle = 20.0f;
	float zoomOutAngle = 50.0f;

	bool isTorchOn = false;

	// main application parameters
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
	
	// load Vulkan Models and Texutures.
	// Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		std::string correctPath = "/Users/harryfoley/Development/computer_graphics/ProjectHunt/hunt";
        chdir(correctPath.c_str());  
        std::cout << "New working directory set: " << correctPath << std::endl;

		// Descriptor Layouts [what will be passed to the shaders]
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
				});
		DSLBlinn.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BlinnUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(BlinnMatParUniformBufferObject), 1}
				});
		DSLOren.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(OrenUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(OrenMatParUniformBufferObject), 1}
				});
		DSLNMap.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(NMapUniformBufferObject), 1},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1},
					{3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(NMapMatParUniformBufferObject), 1}
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
    			{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
				{1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(HUDParUniformBufferObject), 1},
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

		VDOren.init(this, {
				  {0, sizeof(OrenVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(OrenVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(OrenVertex, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(OrenVertex, UV),
				         sizeof(glm::vec2), UV}
				});

		VDNMap.init(this, {
				  {0, sizeof(NMapVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(NMapVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(NMapVertex, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(NMapVertex, UV),
				         sizeof(glm::vec2), UV},
 				{0, 3, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(NMapVertex, tan), sizeof(glm::vec4), TANGENT}

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

		// Pipelines [Shader couples]
		PBlinn.init(this, &VDBlinn,  "shaders/BlinnVert.spv",    "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
		POren.init(this, &VDOren,  "shaders/OrenVert.spv",    "shaders/OrenFrag.spv", {&DSLGlobal, &DSLOren});
		PNMAp.init(this, &VDNMap,  "shaders/NMapVert.spv", "shaders/NMapFrag.spv", {&DSLGlobal, &DSLNMap});		
		PEmission.init(this, &VDEmission,  "shaders/EmissionVert.spv",    "shaders/EmissionFrag.spv", {&DSLEmission});
		PskyBox.init(this, &VDskyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLskyBox});
		PskyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_BACK_BIT, false);
		PHUD.init(this, &VDHUD, "shaders/HUDVert.spv", "shaders/HUDFrag.spv", {&DSLHUD});
        PHUD.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, true);

		// Create models
        MAx.init(this, &VDBlinn, "models/axis.obj", OBJ);
		Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		Mmoon.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		Mground.init(this, &VDBlinn, "models/LargePlane.obj", OBJ);
		MBall.init(this, &VDBlinn, "models/Sphere.obj", OBJ);
		MskyBox.init(this, &VDskyBox, "models/SkyBoxCube.obj", OBJ);
		MGun.init(this, &VDBlinn, "models/gun.obj", OBJ);
		MTower.init(this, &VDNMap, "models/structure/tower.gltf", GLTF);

        MAnimals[0].init(this, &VDBlinn, "models/animals/rhinoceros_001.mgcg", MGCG);
        MAnimals[1].init(this, &VDBlinn, "models/animals/tiger_001.mgcg", MGCG);
        MAnimals[2].init(this, &VDBlinn, "models/animals/wolf_002.mgcg", MGCG);
        MAnimals[3].init(this, &VDBlinn, "models/animals/zebra_001.mgcg", MGCG);
        MAnimals[4].init(this, &VDBlinn, "models/animals/bear_001.mgcg", MGCG);
        MAnimals[5].init(this, &VDBlinn, "models/animals/bison_001.mgcg", MGCG);
        MAnimals[6].init(this, &VDBlinn, "models/animals/bull_001.mgcg", MGCG);
        MAnimals[7].init(this, &VDBlinn, "models/animals/duck_001.mgcg", MGCG);
        MAnimals[8].init(this, &VDBlinn, "models/animals/goat_001.mgcg", MGCG);
        MAnimals[9].init(this, &VDBlinn, "models/animals/lion_female_001.mgcg", MGCG);

        MVegRocks[0].init(this, &VDOren, "models/VegRocks/vegetation.001.mgcg", MGCG);
        MVegRocks[1].init(this, &VDOren, "models/VegRocks/vegetation.018.mgcg", MGCG);
        MVegRocks[2].init(this, &VDOren, "models/VegRocks/vegetation.022.mgcg", MGCG);
        MVegRocks[3].init(this, &VDOren, "models/VegRocks/vegetation.025.mgcg", MGCG);
        MVegRocks[4].init(this, &VDOren, "models/VegRocks/vegetation.044.mgcg", MGCG); //rock tree
        MVegRocks[5].init(this, &VDOren, "models/VegRocks/vegetation.045.mgcg", MGCG); //big rock
        MVegRocks[6].init(this, &VDOren, "models/VegRocks/vegetation.048.mgcg", MGCG); // rock grass
        MVegRocks[7].init(this, &VDOren, "models/VegRocks/vegetation.049.mgcg", MGCG); // rock grass
        MVegRocks[8].init(this, &VDOren, "models/VegRocks/vegetation.mgcg", MGCG); // rock grass
        MVegRocks[9].init(this, &VDOren, "models/VegRocks/vegetation.100.mgcg", MGCG); // rock grass
        MVegRocks[10].init(this, &VDOren, "models/VegRocks/vegetation.103.mgcg", MGCG); // rock grass
        MVegRocks[11].init(this, &VDOren, "models/VegRocks/vegetation.106.mgcg", MGCG); // rock grass

        MStructures[0].init(this, &VDBlinn, "models/structure/cottage.obj", OBJ);
        MStructures[1].init(this, &VDBlinn, "models/structure/fence.obj", OBJ);
        MStructures[2].init(this, &VDBlinn, "models/structure/fence.obj", OBJ);
        MStructures[3].init(this, &VDBlinn, "models/structure/woodhouse.obj", OBJ);

		// building up vertices and indices for HUD crosshairs to use
    	int mainStride = sizeof(HUDVertex);  
		std::vector<HUDVertex> quadVertices = {
		    {{-0.1f, -0.1f}, {0.0f, 0.0f}},  
		    {{ 0.1f, -0.1f}, {1.0f, 0.0f}},  
		    {{ 0.1f,  0.1f}, {1.0f, 1.0f}},  
		    {{-0.1f,  0.1f}, {0.0f, 1.0f}},
		};
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

		std::vector<HUDVertex> vertices;
		std::vector<unsigned int> indices;
		float radius = 0.35f; 
		int segmentCount = 64;
		generateScopeVertices(vertices, glm::vec2(0.0f, 0.0f), radius, segmentCount, Ar);
		generateScopeIndices(indices, segmentCount);

		MScope.vertices.insert(MScope.vertices.end(), reinterpret_cast<unsigned char*>(vertices.data()), reinterpret_cast<unsigned char*>(vertices.data()) + vertices.size() * sizeof(HUDVertex));
		MScope.indices.insert(MScope.indices.end(), indices.begin(), indices.end());
		MScope.initMesh(this, &VDHUD);
 
		// Menu Screen Model vertices and indices defintion

		// vertices
		std::vector<HUDVertex> menuScreenVertices = {
		    {{-1.0f, -1.0f}, {0.0f, 0.0f}},  
		    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},  
		    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},  
		    {{-1.0f,  1.0f}, {0.0f, 1.0f}},
		};
		for (const auto& vertex : menuScreenVertices) {
		    std::vector<unsigned char> menuScreenVertexData(mainStride, 0);
		    HUDVertex* MSV_vertex = (HUDVertex*)(&menuScreenVertexData[0]);
		    
		    MSV_vertex->pos = vertex.pos;
		    MSV_vertex->UV = vertex.UV;
		    
		    MMenuScreen.vertices.insert(MMenuScreen.vertices.end(), menuScreenVertexData.begin(), menuScreenVertexData.end());
		}
		// indices
		std::vector<unsigned int> msquadIndices = {0, 1, 2, 2, 3, 0};
		MMenuScreen.indices.insert(MMenuScreen.indices.end(), msquadIndices.begin(), msquadIndices.end());
		MMenuScreen.initMesh(this, &VDHUD);


        // Create the textures
		Tsun.init(this, "textures/2k_sun.jpg");
		Tmoon.init(this, "textures/2k_moon.jpg");
		Tground.init(this, "textures/2k_sun.jpg");
        T1.init(this,   "textures/Textures.png");
        Tanimal.init(this, "textures/Textures-Animals.png");
        TCrosshair.init(this, "textures/cros.png");
        TGun.init(this, "textures/gun.png");
        TGrass.init(this, "textures/grass1.jpg");
		TTowerNMap.init(this, "textures/Tower_Nor.jpg", VK_FORMAT_R8G8B8A8_UNORM);
		TTowerDiff.init(this, "textures/Tower_Col.jpg");	
		TScope.init(this, "textures/scope.png");	
		TMenuScreen.init(this, "textures/black_background.jpg");
        
        TStructures[0].init(this, "textures/cottage_diffuse.png");
        TStructures[1].init(this, "textures/fenceDiffuse.jpg");
        TStructures[2].init(this, "textures/fenceDiffuse.jpg");
        TStructures[3].init(this, "textures/woodenhousediffuse.png");

		TskyBox.init(this, "textures/starmap_g4k.jpg");
		Tstars.init(this, "textures/bluecloud_up.jpg");


		// from objectInstances
		placeVegRocks(vegRocks);
		placeAnimals(animals);
		placeStructures(structures);

        // balls used for debugging
        for(int i = 0; i < NBALLS; i ++){
	        balls.emplace_back(glm::vec3(0.0f + i, 0.0f, 0.0f), i, glm::vec3(0.1f, 0.1f, 0.1f), 0.0f, "ball");
        }

		// Descriptor pool sizes
		DPSZs.uniformBlocksInPool = 600;
		DPSZs.texturesInPool = 500;
		DPSZs.setsInPool = 500;

		// Texts initialization
		std::cout << "Initializing text\n";
		// menu screen
		outText = {
			{16, {"Welcome user!!", "",
				"This is a hunting game where your current goal is to catch all of the animals",
				"in the shortest time possible.", "", "",
				"You have been given a rifle:", "",
				"- Press <W, A,S, D> to move",
				"- Press <SPACE_BAR> to shoot.",
				"- Press <Z> to zoom in and out.",
				"- Press <U> to turn on/off the torch.",
				"- Press the arrow keys or use the mouse to look around and aim at your target.", "", "",
				"Whenever you're ready, press <ENTER> to start the game!"}, 0, 0},
			{0, {"","","",""}, 0, 0},
			{16, {"YOU WIN!!", "",
				"Well done, you found all the animals in the map!!", "",
				"You managed to complete your task in the time given.", "", "", "", "", "", "", "", "",
				"Press <ENTER> to start a new game.", "",
				"Press <ESC> to exit."}, 0, 0},
				{16, {"GAMEOVER :(", "",
				"I'm sorry, but time is up", "",
				"You didn't manage to completed your task in the time given.", "", "", "", "", "", "", "", "",
				"Press <ENTER> to start a new game.", "",
				"Press <ESC> to exit."}, 0, 0},
		};

		// time counter
		char bufTime[GAMEDURATION][100];
		outTimeText.push_back({1, "", 0, 0});
		for (int i = 1; i <= GAMEDURATION; i++) {
			strcpy(bufTime[i], "Time left: ");
			strcat(bufTime[i], std::to_string(i).c_str());
			strcat(bufTime[i], " seconds");
        	outTimeText.push_back({1, bufTime[i], 0, 0});	
		}

		// animal counter
		char bufAnimal[NANIMAL][100];
		outAnimalText.push_back({3, {"", "", ""}, 0, 0});
		for (int i = 1; i <= NANIMAL; i++) {
			strcpy(bufAnimal[i], "Animals left: ");
			strcat(bufAnimal[i], std::to_string(i).c_str());
        	outAnimalText.push_back({3, {"", "", bufAnimal[i]}, 0, 0});	
		}

		// actual initialization
		txt.init(this, &outText);
		timeTxt.init(this, &outTimeText);
		animalTxt.init(this, &outAnimalText);
		
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
		PNMAp.create();
		POren.create();
		PEmission.create();
		PskyBox.create();
		PHUD.create();
        
		// Here you define the data set
		DSsun.init(this, &DSLEmission, {&Tsun});
		DSmoon.init(this, &DSLEmission, {&Tmoon});
		DSground.init(this, &DSLBlinn, {&TGrass});
        DSAx.init(this, &DSLBlinn, {&T1});
        DSCrosshair.init(this, &DSLHUD, {&TCrosshair});
		DSScope.init(this, &DSLHUD, {&TScope});
		DSskyBox.init(this, &DSLskyBox, {&TskyBox, &Tstars});
		DSGun.init(this, &DSLBlinn, {&TGun});
		DSMenuScreen.init(this, &DSLHUD, {&TMenuScreen});
		DSTower.init(this, &DSLNMap, {&TTowerDiff, &TTowerNMap});

		// DSGameOver.init(this, &DSLHUD, {&TGameOver});

        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.init(this, &DSLBlinn, {&Tanimal});
        };

        for (DescriptorSet &DSVegRock: DSVegRocks) {
            DSVegRock.init(this, &DSLOren, {&T1});
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
		timeTxt.pipelinesAndDescriptorSetsInit();
		animalTxt.pipelinesAndDescriptorSetsInit();

		std::cout << "Descriptor Set init!\n";

	}

	// destroy pipelines and Descriptor Sets
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PBlinn.cleanup();
		PNMAp.cleanup();
		POren.cleanup();
		PEmission.cleanup();
		PHUD.cleanup();
		PskyBox.cleanup();

		DSsun.cleanup();
		DSmoon.cleanup();
		DSGlobal.cleanup();
		DSground.cleanup();
        DSAx.cleanup();
        DSCrosshair.cleanup();
		DSScope.cleanup();
		DSskyBox.cleanup();
		DSGun.cleanup();
		DSTower.cleanup();
		DSMenuScreen.cleanup();

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
		timeTxt.pipelinesAndDescriptorSetsCleanup();
		animalTxt.pipelinesAndDescriptorSetsCleanup();

        std::cout << "Descriptor Set cleanup!\n";
	}


	void localCleanup() {	
		TTowerNMap.cleanup();
		TScope.cleanup();
		Tsun.cleanup();
		Tmoon.cleanup();
		Tground.cleanup();
        T1.cleanup();
        Tanimal.cleanup();
        TCrosshair.cleanup();
        TGun.cleanup();	
		TMenuScreen.cleanup();
		TTowerDiff.cleanup();
		TskyBox.cleanup();
		Tstars.cleanup();

        for (Texture &Tstruct: TStructures) {
            Tstruct.cleanup();
        }

		MCrosshair.cleanup();
		MScope.cleanup();
		Msun.cleanup();
		Mmoon.cleanup();
		Mground.cleanup();
        MAx.cleanup();
        MBall.cleanup();
        MGun.cleanup();
		MMenuScreen.cleanup();
		MTower.cleanup();
		MskyBox.cleanup();

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
		DSLNMap.cleanup();
		DSLOren.cleanup();
		DSLEmission.cleanup();
		DSLGlobal.cleanup();
		DSLHUD.cleanup();
		DSLskyBox.cleanup();
		
		// Destroies the pipelines
		PBlinn.destroy();
		PNMAp.destroy();
		POren.destroy();
		PEmission.destroy();
		PHUD.destroy();
		PskyBox.destroy();

		txt.localCleanup();	
		timeTxt.localCleanup();	
		animalTxt.localCleanup();
	}
	
	// creation of the command buffer:
	// send to the GPU all the objects you want to draw,
	// with their buffers and textures

	void shootGun(){
				::printVec3(ray.direction);
		float t0, t1;
		for (int index = 0; index < NANIMAL; index++) {
			if(animals[index].visible){
				if(rayIntersectsSphere(ray.origin, ray.direction, animals[index].pos, 0.5, t0, t1)){ 
					std::cout<< "Sphere HIT" << std::endl;
					animals[index].visible = false;
					aliveAnimals--;
					currAnimalIndex --;
					if (aliveAnimals == 0) {
						currScene = GAMEWIN;
						currTimeIndex = 0;
					}
					RebuildPipeline();
				}
			}
		}
	}
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		// Oren-Nayar Pipeline
		POren.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, POren, 0, currentImage);
		for (int index = 0; index < NVEGROCK; index++) {
		    const Instance& instance = vegRocks[index];
            MVegRocks[instance.modelID].bind(commandBuffer);
            DSVegRocks[index].bind(commandBuffer, POren, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MVegRocks[instance.modelID].indices.size()), 1, 0, 0, 0);
        }

		// Normal Map Pipeline
		PNMAp.bind(commandBuffer);
		MTower.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PNMAp, 0, currentImage);
		DSTower.bind(commandBuffer, PNMAp, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MTower.indices.size()), 1, 0, 0, 0);
        
		// Blinn Pipeline
		PBlinn.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PBlinn, 0, currentImage);	// The Global Descriptor Set (Set 0)

		for (int index = 0; index < NANIMAL; index++) {
		    const Instance& instance = animals[index];
            MAnimals[instance.modelID].bind(commandBuffer);
            DSAnimals[index].bind(commandBuffer, PBlinn, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MAnimals[instance.modelID].indices.size()), 1, 0, 0, 0);
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

		// Emission Pipeline
		PEmission.bind(commandBuffer);
		Msun.bind(commandBuffer);
		DSsun.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);	

		Mmoon.bind(commandBuffer);
		DSmoon.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mmoon.indices.size()), 1, 0, 0, 0);

		// SkyBox Pipeline
		PskyBox.bind(commandBuffer);
		MskyBox.bind(commandBuffer);
		DSskyBox.bind(commandBuffer, PskyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MskyBox.indices.size()), 1, 0, 0, 0);

		// HUD Pipeline
		PHUD.bind(commandBuffer);
		MCrosshair.bind(commandBuffer);
		DSCrosshair.bind(commandBuffer, PHUD, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCrosshair.indices.size()), 1, 0, 0, 0);	

		MScope.bind(commandBuffer);
		DSScope.bind(commandBuffer, PHUD, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MScope.indices.size()), 1, 0, 0, 0);	

		MMenuScreen.bind(commandBuffer);
		DSMenuScreen.bind(commandBuffer, PHUD, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MMenuScreen.indices.size()), 1, 0, 0, 0);

		// Text Pipeline
		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
		timeTxt.populateCommandBuffer(commandBuffer, currentImage, currTimeIndex);
		animalTxt.populateCommandBuffer(commandBuffer, currentImage, currAnimalIndex);
	}

	void checkPlayerObjectCollisions(glm::vec3& playerPos, glm::vec3&  firstPlayerPos, glm::vec3& lastPlayerPos){
		for( auto vegRock : vegRocks){
			float objRadius = 1.0f;
			if(vegRock.desc == "Rock"){
				objRadius = 2.0f;
			}
			if (checkCollision(playerPos + firstPlayerPos, 0.5f, vegRock.pos, objRadius)) {
				//std::cout << "collided" << "\n";
				//std::cout << vegRocks[index].scale.x << "\n";
				//std::cout << vegRocks[index].desc << "\n";
				glm::vec3 collision_direction = glm::normalize( (playerPos + firstPlayerPos) - vegRock.pos);
				collision_direction = glm::vec3(collision_direction.x, 0.0f, collision_direction.z);
				float pushback_distance = (2.5f) - glm::length((playerPos + firstPlayerPos) - vegRock.pos);
				playerPos = lastPlayerPos;  // stick 
			}					
		}
		
		if (checkCollision(playerPos + firstPlayerPos, 0.5f,towerPos, 1.3f )) {
			glm::vec3 collision_direction = glm::normalize( (playerPos + firstPlayerPos) - towerPos);
			collision_direction = glm::vec3(collision_direction.x, 0.0f, collision_direction.z);
			float pushback_distance = (2.5f) - glm::length((playerPos + firstPlayerPos) - towerPos);
			playerPos = lastPlayerPos;  // stick 
		}
	}

	void updatePlayerPos(float& CamPitch, float& CamYaw, float deltaT, glm::vec3 m, glm::vec3 r, float ROT_SPEED){
		const float MOVE_SPEED = 10.0f;
		
		if (!isDebugMode)
			{
				glm::vec3 FirstPos = glm::vec3(2, 1, 5);
				glm::vec3 Pos = glm::vec3(0, 0, 0);
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
				CamPitch -= ROT_SPEED*0.5f * deltaT * r.x;
				
				Pos = lastPos + ux * dampedVelx + uz * dampedVelz;

				// fence perimeter bounds
				if(Pos.x > 40.5f){
					//std::cout << "Pos x too big "<< Pos.x << "\n";
					Pos = glm::vec3( 40.5f, Pos.y, Pos.z);
				}
				if(Pos.x < -41.5f){
					//std::cout << "Pos x too small "<< Pos.x << "\n";
					Pos = glm::vec3( -41.5f, Pos.y, Pos.z);
				}
				if(Pos.z > 40.0f){
					//std::cout << "Pos z too big "<< Pos.z << "\n";
					Pos = glm::vec3( Pos.x, Pos.y, 40.0f);
				}
				if(Pos.z < -44.5f){
					//std::cout << "Pos z too small "<< Pos.z << "\n";
					Pos = glm::vec3( Pos.x, Pos.y, -44.5f);
				}

				checkPlayerObjectCollisions(Pos, FirstPos, lastPos);
			
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

	void handleInput(float cTime, float tTime){
		static bool debounce = false;
		static int curDebounce = 0; 
		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;		
				// Shoots only during the match
				if (currScene == MATCH) {
					shootGun();
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

		// Zoom in and out
		if(glfwGetKey(window, GLFW_KEY_Z)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_Z;
				std::cout<< cameraZoom << "\n";

				if(cameraZoom == zoomInAngle){
					cameraZoom = zoomOutAngle;
					std::cout<< "zoom out" << "\n";
					RebuildPipeline();

				}
				else if (cameraZoom == zoomOutAngle){
					cameraZoom = zoomInAngle;
					std::cout<< "zoom in" << "\n";
					RebuildPipeline();
				}

			}
		} else {
			if((curDebounce == GLFW_KEY_Z) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
	

		if(glfwGetKey(window, GLFW_KEY_V)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printMat4("ViewMatrix  ", ViewMatrix);				
				std::cout << "cTime    = " << cTime    << ";\n";
				std::cout << "tTime    = " << tTime    << ";\n";
				printVec3("User Dir  = ", extractPlayerDirectionFromViewMatrix(ViewMatrix));
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Torch Light On/Off
		if(glfwGetKey(window, GLFW_KEY_U)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_U;

				isTorchOn = !isTorchOn;
				std::cout << "Torch On/Off button toggled\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_U) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		// Go to the next scene
		if(glfwGetKey(window, GLFW_KEY_ENTER)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_ENTER;

				if (currScene != MATCH) {
					currScene = MATCH;
					for (int index = 0; index < NANIMAL; index++) {
						animals[index].visible = true;
					}
					aliveAnimals = NANIMAL;
					startTime = cTime;
					currAnimalIndex = NANIMAL;
					lastTime = 0.0f;
					currTimeIndex = GAMEDURATION;
					RebuildPipeline();
				}
				std::cout << "Scene : " << currScene << "\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_ENTER) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		}
	}
	
	// update the uniforms.
	// game logic
	void updateUniformBuffer(uint32_t currentImage) {
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		
		static float autoTime = true;
		static float cTime = 0.0;
		float turnTime = 72.0f * 10.0f;
		if (!isDebugMode)
			turnTime = 72.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		
		if(autoTime) {
			cTime = cTime + deltaT;
		}

		static float tTime = 0.0;
		const float TturnTime = 60.0f;
		const float TangTurnTimeFact = 1.0f / TturnTime;
		
		if(autoTime) {
			tTime = tTime + deltaT;
			tTime = (tTime > TturnTime) ? (tTime - TturnTime) : tTime;
		}

		// Day and night cycle through time
		float dayTime = sin(cTime * angTurnTimeFact + dayCyclePhase);
		
		float ROT_SPEED = glm::radians(120.0f);

		// Differnet rotation speeds for zoom in and out
		if (cameraZoom==zoomOutAngle) {
			ROT_SPEED = ZOUT_ROT_SPEED;
		} else {
			ROT_SPEED = ZIN_ROT_SPEED;
		}
		
		static float CamPitch = glm::radians(0.0f);
		static float CamYaw   = M_PI;

		HUDParUniformBufferObject crosshairParUBO = {};
		HUDParUniformBufferObject menuScreenParUBO = {};

		// HUD elements transparency
		crosshairParUBO.alpha = 1.0f;
		menuScreenParUBO.alpha = 0.9f;

		if (currScene == STARTMENU) {
			
			crosshairParUBO.visible = false;
			menuScreenParUBO.visible = true;

		} else if (currScene == MATCH) {

			crosshairParUBO.visible = true;
			menuScreenParUBO.visible = false;
			
		updatePlayerPos(CamPitch, CamYaw, deltaT, m, r, ROT_SPEED);

		} else if (currScene == GAMEOVER || currScene == GAMEWIN) {

			crosshairParUBO.visible = false;
			menuScreenParUBO.visible = true;

		} else {
			std::cout << "Error, wrong scene : " << currScene << "\n";
		}

		// Updates descriptor sets
		DSCrosshair.map(currentImage, &crosshairParUBO, 1);

		if(cameraZoom == zoomInAngle){
			DSScope.map(currentImage, &crosshairParUBO, 1);
		}

		DSMenuScreen.map(currentImage, &menuScreenParUBO, 1);

		handleInput(cTime, tTime);

		// update  uniforms
		glm::mat4 M = glm::perspective(glm::radians(cameraZoom), Ar, 0.1f, 160.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = ViewMatrix;
		glm::mat4 ViewPrj =  M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);		
		ray = calculateRayFromScreenCenter(Mv, M);	

		if (currScene==MATCH) {

			currTime = cTime - startTime;

			// Time counter update
			if (currTime > lastTime + UPDATETIME) {

				currTimeIndex = GAMEDURATION - (int) currTime;
				lastTime = currTime;
				RebuildPipeline();
			}
			// game over condition
			if (currTime > (float) GAMEDURATION) {

				currTimeIndex = 0;
				currAnimalIndex = 0;
				currScene = GAMEOVER;
				RebuildPipeline();
			}
		}

		// updates global uniforms
		// Global
		GlobalUniformBufferObject gubo{};

		// Direct light parameters
		gubo.lightDir = glm::vec3(cos(cTime * angTurnTimeFact + dayCyclePhase), dayTime, 0.0f);
		float intensity = glm::min(0.0f, dayTime);
		float maxDegree = 0.342f;
		float dayIntensity = dayTime;
		float nightIntensity = -dayTime;

		// Day and night intensities update
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
		
		// Sunlight color (with sunset and sunrise)
		gubo.dayLightColor = glm::vec4(
			1.0f * dayIntensity, 
			(0.2f + 0.8f * abs(dayTime)) * dayIntensity, 
			abs(dayTime * dayIntensity),
			1.0f
			);
		// Moonlight color
		gubo.nightLightColor = glm::vec4(
			0.1f * nightIntensity * 0.25f, 
			0.4f * nightIntensity * 0.25f, 
			0.4f * nightIntensity * 0.25f, 
			1.0f
			);

		// User position and direction (torch light)
		gubo.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));
		gubo.userDir = extractPlayerDirectionFromViewMatrix(ViewMatrix);

		gubo.ambient = dayTime;		// Dyinamic ambient light

		// Torch light toggle
		if (isTorchOn)
			gubo.pointLightColor = glm::vec3(1.0f, 1.0f, 0.3f);	// On
		else
			gubo.pointLightColor = glm::vec3(0.0f);				// Off

		// Spotlight paramters
		gubo.gFactor = G_FACTOR;
		gubo.beta = BETA;
		gubo.cIn = C_IN;
		gubo.cOut = C_OUT;

		DSGlobal.map(currentImage, &gubo, 0);

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


		BlinnUniformBufferObject blinnUbo{};
		BlinnMatParUniformBufferObject blinnMatParUbo{};   
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

		blinnMatParUbo.Power = 20.0;
		blinnMatParUbo.scaleUV = 1.0;

		if(cameraZoom == zoomOutAngle){
			DSGun.map(currentImage, &blinnUbo, 0);
			DSGun.map(currentImage, &blinnMatParUbo, 2);
		}

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
		
		DSGlobal.map(currentImage, &gubo, 0);

		OrenUniformBufferObject orenUbo{};
		OrenMatParUniformBufferObject orenMatParUbo{};

        orenUbo.mMat = glm::mat4(1);
        orenUbo.nMat = glm::mat4(1);
        orenUbo.mvpMat = ViewPrj;
		orenMatParUbo.scaleUV = 1.0;
		orenMatParUbo.Roughness = 0.5;

		// VEG ROCKs
        for (int model = 0; model < NVEGROCK; model++) {
        	const Instance& instance = vegRocks[model];
 			orenUbo.mMat = glm::translate(glm::mat4(1.0f),
                                           instance.pos) 
 											* glm::rotate(glm::mat4(1.0f), glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f))
 											* glm::rotate(glm::scale(glm::mat4(1), instance.scale), glm::radians(instance.angle),glm::vec3(0.0f, 1.0f, 0.0f)) * baseTr;
        	orenUbo.mvpMat = ViewPrj * orenUbo.mMat;
        	orenUbo.nMat = glm::inverse(glm::transpose(orenUbo.mMat));

            DSVegRocks[model].map(currentImage, &orenUbo, 0);
        	DSVegRocks[model].map(currentImage, &orenMatParUbo, 2);
        }

		NMapUniformBufferObject nmapUbo{};
		NMapMatParUniformBufferObject nmapMatParUbo{};

        nmapUbo.mMat = glm::mat4(1);

		nmapUbo.mMat = glm::translate(glm::mat4(1.0f),
								towerPos) 
								* glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),glm::vec3(0.0f, 1.0f, 0.0f))
								* glm::rotate(glm::mat4(1), glm::radians(0.0f),glm::vec3(1.0f, 0.0f, 0.0f)) * baseTr;

		nmapUbo.mvpMat = ViewPrj * nmapUbo.mMat;
		nmapUbo.nMat = glm::inverse(glm::transpose(nmapUbo.mMat));
		nmapMatParUbo.scaleUV = 1.0;
		nmapMatParUbo.Roughness = 0.5;

		DSTower.map(currentImage, &nmapUbo, 0);
		DSTower.map(currentImage, &nmapMatParUbo, 3);
	}
};

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
    } else {
        std::cout << "Running in Release mode" << std::endl;
    }
	auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    unsigned int seed = static_cast<unsigned int>(duration.count());
    srand(seed);
    std::cout << "Seed: " << seed << std::endl;

    dayCyclePhase = ((float)rand() / RAND_MAX) * (2.0f * M_PI);

    std::cout << "dayCyclePhase: " << dayCyclePhase << std::endl;
	
    HuntGame app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
