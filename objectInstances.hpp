#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <math.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>

// class used to create instances of an object
class Instance {
public:
    glm::vec3 pos;      // Instance position
    int modelID;        // Model ID used to get the model of the object
    glm::vec3 scale;    // Scale
    float angle;        // Angle for rotation
    std::string desc;   // Description of the instance
    bool visible = true;
	float angle2 = 0.0f;

    Instance(const glm::vec3& position, int id, const glm::vec3& scl, float ang, const std::string& description)
        : pos(position), modelID(id), scale(scl), angle(ang), desc(description) {}
};


void placeAnimals(std::vector<Instance>& animals){
        //ANIMALS
		animals.emplace_back(glm::vec3(2.0f, 0.0f, 10.0f), 0, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(5.0f, 0.0f, 30.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(7.0f, 0.0f, 6.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(9.0f, 0.0f, 20.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(11.0f, 0.0f, 0.0f), 4, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(14.0f, 0.0f, 5.0f), 5, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(25.0f, 0.0f, 14.0f), 6, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(37.0f, 0.0f, 30.0f), 7, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(19.0f, 0.0f, 20.0f), 8, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(31.0f, 0.0f, 09.0f), 9, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(-38.0f, 0.0f, 20.0f), 0, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-5.0f, 0.0f, 30.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-17.0f, 0.0f, 6.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-9.0f, 0.0f, 20.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-11.0f, 0.0f, 0.0f), 4, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(-39.0f, 0.0f, 5.0f), 5, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-25.0f, 0.0f, 14.0f), 6, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-37.0f, 0.0f, 33.0f), 7, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-19.0f, 0.0f, 38.0f), 8, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-31.0f, 0.0f, 09.0f), 9, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(-10.0f, 0.0f, -22.0f), 4, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-25.0f, 0.0f, -30.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-37.0f, 0.0f, -6.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-19.0f, 0.0f, -22.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-31.0f, 0.0f, -10.0f), 0, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(-39.0f, 0.0f, -15.0f), 9, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-25.0f, 0.0f, -4.0f), 8, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-37.0f, 0.0f, -13.0f), 7, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-19.0f, 0.0f, -28.0f), 6, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(-31.0f, 0.0f, -19.0f), 5, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(10.0f, 0.0f, -32.0f), 4, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(25.0f, 0.0f, -30.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(37.0f, 0.0f, -26.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(19.0f, 0.0f, -22.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(31.0f, 0.0f, -10.0f), 0, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

		animals.emplace_back(glm::vec3(29.0f, 0.0f, 15.0f), 9, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(35.0f, 0.0f, 4.0f), 8, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(7.0f, 0.0f, 13.0f), 7, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(19.0f, 0.0f, 28.0f), 6, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");
		animals.emplace_back(glm::vec3(36.0f, 0.0f, 19.0f), 5, glm::vec3(1.0f, 1.0f, 1.0f), randomFloat(0.0f, 360.0f), "animal");

}

void placeVegRocks(std::vector<Instance>& vegRocks){
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
        vegRocks.emplace_back(glm::vec3(40.0f, 0.0f, -32.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(20.0f, 0.0f, -29.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(10.0f, 0.0f, -30.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(-5.0f, 0.0f, -38.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(-22.0f, 0.0f, -25.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");

        vegRocks.emplace_back(glm::vec3(-43.0f, 0.0f, -40.0f), 1,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");

        vegRocks.emplace_back(glm::vec3(-33.0f, 0.0f, -30.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(-23.0f, 0.0f, -10.0f), 1,glm::vec3(1.5f, 2.0f, 1.5f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(-40.0f, 0.0f, 	6.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(-30.0f, 0.0f, 14.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");

        vegRocks.emplace_back(glm::vec3(-38.0f, 0.0f, 40.0f), 1,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Tree");

        vegRocks.emplace_back(glm::vec3(-18.0f, 0.0f, 40.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Tree");
        vegRocks.emplace_back(glm::vec3( 0.0f, 0.0f, 20.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Tree");
        vegRocks.emplace_back(glm::vec3( 8.0f, 0.0f, 33.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Tree");
        vegRocks.emplace_back(glm::vec3( 22.0f, 0.0f, 41.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 180.0f, "Tree");

        vegRocks.emplace_back(glm::vec3(40.0f, 0.0f, 20.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 90.0f, "Tree");

        vegRocks.emplace_back(glm::vec3(30.0f, 0.0f, -30.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(43.0f, 0.0f, -10.0f), 1,glm::vec3(1.5f, 2.0f, 1.5f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(23.0f, 0.0f, 	6.0f), 3,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
        vegRocks.emplace_back(glm::vec3(43.0f, 0.0f, 14.0f), 2,glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");

		// three trees 1 rock
		vegRocks.emplace_back(glm::vec3(5.0f, 0.0f, -5.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
		vegRocks.emplace_back(glm::vec3(2.0f, 0.0f, -2.0f), 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
		vegRocks.emplace_back(glm::vec3(4.0f, 0.0f, -1.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Tree");
		vegRocks.emplace_back(glm::vec3(3.0f, 0.0f, -3.0f), 10, glm::vec3(1.50f, 3.0f, 1.5f), 0.0f, "Tree");

		vegRocks.emplace_back(glm::vec3(-10.0f, -1.0f, -5.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 120.0f, "Tree");
		vegRocks.emplace_back(glm::vec3(-13.0f, -2.0f, -5.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 120.0f, "Tree");
		vegRocks.emplace_back(glm::vec3(-11.0f, 0.0f, -8.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 120.0f, "Tree");

		vegRocks.emplace_back(glm::vec3( 15.0f, -2.0f, 10.0f), 5, glm::vec3(1.0f, 1.0f, 1.0f), 120.0f, "Rock");

}

void placeStructures(std::vector<Instance>& structures){
        /// STRUCTURES


		// Fences 
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(-40.0f +  2.5f*i, 0.0f, -40.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence1");
		}
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(-40.0f +  2.5f*i, 0.0f, 45.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence1"); //direction of spawning
		}
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(43.0, 0.0f, -38.0f +  2.5f*i), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence2");
			structures[68+i].angle2 = 90.0f;
		}	
		for(int i = 0; i < 34; i++){
			structures.emplace_back(glm::vec3(-40.0, 0.0f, -37.6f +  2.5f*i), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence2");
			structures[102+i].angle2 = 90.0f;
		}		

        //structures.emplace_back(glm::vec3(0.0f, 0.0f, 10.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "Cottage");
        //structures.emplace_back(glm::vec3(10.0f, 0.0f, -10.0f), 3, glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, "House");
        structures.emplace_back(glm::vec3(4.0f, 0.0f, -10.0f), 2, glm::vec3(0.01f, 0.01f, 0.01f), 0.0f, "lightpole");
        //structures.emplace_back(glm::vec3(5.5f, 0.0f, -10.0f), 1, glm::vec3(0.01f, 0.01f, 0.01f), -90.0f, "fence");
}