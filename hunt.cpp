// This has been adapted from the Vulkan tutorial
// And from A10

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{2, {"Adding an object", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
};

// The uniform buffer object used in this example
#define NSHIP 1
#define NANIMAL 10



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

struct skyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};


// Place here the CPP struct for the uniform buffer for the matrices

struct UniformBufferObject {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

// Place here the CPP struct for the uniform buffer for the parameters
//
//struct NormalMapParUniformBufferObject {
//    alignas(4) float pow;
//    alignas(4) float ang;
//    alignas(4) float showCloud;
//    alignas(4) float showTexture;
//};

struct alignas(16) NormalMapParUniformBufferObject {
    float pow;        // Offset: 0, Size: 4 bytes
    float ang;        // Offset: 4, Size: 4 bytes
    float showCloud;  // Offset: 8, Size: 4 bytes
    float showTexture;// Offset: 12, Size: 4 bytes
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

struct skyBoxVertex {
	glm::vec3 pos;
};


// MAIN ! 
class HuntGame : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal;	    // For Global values
	DescriptorSetLayout DSLBlinn;	    // For Blinn Objects
	DescriptorSetLayout DSLEmission;	// For Emission Objects

	// Vertex formats
	VertexDescriptor VDBlinn;
	VertexDescriptor VDEmission;

	// Pipelines [Shader couples]
	Pipeline PBlinn;
	Pipeline PEmission;

	// Scenes and texts
    TextMaker txt;
    
    // Models
    Model MAx; //axis
    Model Mship;
    Model Msun;
    Model Mground;

    Model MAnimals[NANIMAL];
    
    // Descriptor Sets
    DescriptorSet DSGlobal, DSAx;
    DescriptorSet DSship;
    DescriptorSet DSsun;
    DescriptorSet DSground;
    DescriptorSet DSAnimals[NANIMAL];
    
   // Textures
    Texture T1, Tanimal;
    Texture Tship;
    Texture Tsun;
    Texture Tground;
    
	
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
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
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

//  Place here the initialization for the VertexDescriptor

              
		// Pipelines [Shader couples]
		PBlinn.init(this, &VDBlinn,  "shaders/BlinnVert.spv",    "shaders/BlinnFrag.spv", {&DSLGlobal, &DSLBlinn});
		PEmission.init(this, &VDEmission,  "shaders/EmissionVert.spv",    "shaders/EmissionFrag.spv", {&DSLEmission});


		// Create models
        MAx.init(this, &VDBlinn, "models/axis.obj", OBJ);
		Mship.init(this, &VDBlinn, "models/elephant_001.mgcg", MGCG);
		Msun.init(this, &VDEmission, "models/Sphere.obj", OBJ);
		Mground.init(this, &VDBlinn, "models/LargePlane.obj", OBJ);
        
        MAnimals[0].init(this, &VDBlinn, "models/rhinoceros_001.mgcg", MGCG);
        MAnimals[1].init(this, &VDBlinn, "models/tiger_001.mgcg", MGCG);
        MAnimals[2].init(this, &VDBlinn, "models/wolf_002.mgcg", MGCG);
        MAnimals[3].init(this, &VDBlinn, "models/zebra_001.mgcg", MGCG);
        MAnimals[4].init(this, &VDBlinn, "models/bear_001.mgcg", MGCG);
        MAnimals[5].init(this, &VDBlinn, "models/bison_001.mgcg", MGCG);
        MAnimals[6].init(this, &VDBlinn, "models/bull_001.mgcg", MGCG);
        MAnimals[7].init(this, &VDBlinn, "models/duck_001.mgcg", MGCG);
        MAnimals[8].init(this, &VDBlinn, "models/elephant_001.mgcg", MGCG);
        MAnimals[9].init(this, &VDBlinn, "models/lion_female_001.mgcg", MGCG);


        // Create the textures
		Tship.init(this, "textures/XwingColors.png");
		Tsun.init(this, "textures/2k_sun.jpg");
		Tground.init(this, "textures/2k_sun.jpg");
        T1.init(this,   "textures/Textures.png");
        Tanimal.init(this, "textures/Textures-Animals.png");


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
        
		// Here you define the data set
		DSship.init(this, &DSLBlinn, {&Tanimal});
		DSsun.init(this, &DSLEmission, {&Tsun});
		DSground.init(this, &DSLBlinn, {&Tground});
        DSAx.init(this, &DSLBlinn, {&T1});

        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.init(this, &DSLBlinn, {&Tanimal});
            std::cout << "DSAnimal.init!\n";
        };

			
		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PBlinn.cleanup();
		PEmission.cleanup();
		DSship.cleanup();
		DSsun.cleanup();
		DSGlobal.cleanup();
		DSground.cleanup();
        DSAx.cleanup();

        for (DescriptorSet &DSAnimal: DSAnimals) {
            DSAnimal.cleanup();
            std::cout << "DSAnimal.cleanup!\n";
        }

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		Tship.cleanup();
		Mship.cleanup();

		Tsun.cleanup();
		Msun.cleanup();

		Tground.cleanup();
		Mground.cleanup();
        
        T1.cleanup();
        Tanimal.cleanup();
        MAx.cleanup();

        for (Model &MAnimal: MAnimals) {
            MAnimal.cleanup();
        }



		
		// Cleanup descriptor set layouts
		DSLBlinn.cleanup();
		DSLEmission.cleanup();
		DSLGlobal.cleanup();
		
		// Destroies the pipelines
		PBlinn.destroy();
		PEmission.destroy();

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
		for (int model = 0; model < NANIMAL; model++) {
            MAnimals[model].bind(commandBuffer);
            DSAnimals[model].bind(commandBuffer, PBlinn, 1, currentImage);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MAnimals[model].indices.size()), 1, 0, 0, 0);
        }
		// DSship.bind(commandBuffer, PBlinn, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)

		// // The actual draw call.
		// vkCmdDrawIndexed(commandBuffer,
		// 		static_cast<uint32_t>(Mship.indices.size()), 1, 0, 0, 0);	

		// Mground.bind(commandBuffer);
		// DSground.bind(commandBuffer, PBlinn, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)
		// vkCmdDrawIndexed(commandBuffer,
		// 		static_cast<uint32_t>(Mground.indices.size()), 1, 0, 0, 0);	

        MAx.bind(commandBuffer);
        DSAx.bind(commandBuffer, PBlinn, 1, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MAx.indices.size()), 1, 0, 0, 0);


        
		PEmission.bind(commandBuffer);
		Msun.bind(commandBuffer);
		DSsun.bind(commandBuffer, PEmission, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Msun.indices.size()), 1, 0, 0, 0);	



            
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
		
		static float autoTime = false;
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
		const float MOVE_SPEED = 2.0f;
		
		static float ShowCloud = 1.0f;
		static float ShowTexture = 1.0f;
		
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
				std::cout << "ShowCloud    = " << ShowCloud    << ";\n";
				std::cout << "ShowTexture    = " << ShowTexture    << ";\n";
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

//		for(int j = 0; j < 2; j++) {
//			for(int k = 0; k < 4; k++) {
//				int i = j*4+k;
//                blinnUbo.mMat[i] = glm::translate(glm::mat4(1),glm::vec3(i,j,k)) * glm::scale(glm::mat4(1), glm::vec3(0.5,0.5,0.5)) * baseTr;
//				blinnUbo.mvpMat[i] = ViewPrj * blinnUbo.mMat[i];
//				blinnUbo.nMat[i] = glm::inverse(glm::transpose(blinnUbo.mMat[i]));
//			}
//		}
        
        int i = 0;
        int j = 2;
        int k = 2;

        // blinnUbo.mMat[i] = glm::translate(
        //                                   glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f)),
        //                                   glm::vec3(0,0,0)) * glm::scale(glm::mat4(1), glm::vec3(1.0,1.0,1.0)) * baseTr;
        // blinnUbo.mvpMat[i] = ViewPrj * blinnUbo.mMat[i];
        // blinnUbo.nMat[i] = glm::inverse(glm::transpose(blinnUbo.mMat[i]));
        // DSship.map(currentImage, &blinnUbo, 0);

		// blinnMatParUbo.Power = 200.0;
        // DSship.map(currentImage, &blinnMatParUbo, 2);


		EmissionUniformBufferObject emissionUbo{};
		emissionUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), gubo.lightDir * 40.0f) * baseTr;
		DSsun.map(currentImage, &emissionUbo, 0);
		
		
//  Add to compute the uniforms and pass them to the shaders. You need two uniforms: one for the matrices, and the other for the material parameters.
             UniformBufferObject matUbo{};
        //     NormalMapParUniformBufferObject normalMapParUbo{};
            
		// // World and normal matrix should be the identiy. The World-View-Projection should be variable ViewPrj
             matUbo.mMat = glm::mat4(1);
             matUbo.nMat = glm::mat4(1);
             matUbo.mvpMat = ViewPrj;
		// // These informations should be used to fill the Uniform Buffer Object in Binding 0 of your DSL
             DSAx.map(currentImage, &matUbo, 0); //DSground
             blinnMatParUbo.Power = 200.0;
        	 DSAx.map(currentImage, &blinnMatParUbo, 2);


        for (int model = 0; model < NANIMAL; model++) {

 			blinnUbo.mMat = glm::translate(
                                          glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f)),
                                          glm::vec3( model , 2, 1)) * glm::scale(glm::mat4(1), glm::vec3(0.5,0.5,0.5)) * baseTr;
        	blinnUbo.mvpMat = ViewPrj * blinnUbo.mMat;
        	blinnUbo.nMat = glm::inverse(glm::transpose(blinnUbo.mMat));

            DSAnimals[model].map(currentImage, &blinnUbo, 0);

			blinnMatParUbo.Power = 200.0;
        	DSAnimals[model].map(currentImage, &blinnMatParUbo, 2);
        }


        //     normalMapParUbo.pow = 200.0;
        //     normalMapParUbo.ang = tTime * TangTurnTimeFact;
        //     normalMapParUbo.showCloud = ShowCloud;

        //     normalMapParUbo.showTexture = ShowTexture;

        //     // These informations should be used to fill the Uniform Buffer Object in Binding 6 of your DSL
        //     DSearth.map(currentImage, &normalMapParUbo, 6);
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    HuntGame app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
