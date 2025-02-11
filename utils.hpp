#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <array>
#include <cmath>
#include <math.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>


float randomFloat(float min, float max) {
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return min + random * (max - min);
}

glm::vec3 extractPlayerPositionFromViewMatrix(const glm::mat4& viewMatrix) {
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
    glm::vec3 playerPosition = glm::vec3(inverseViewMatrix[3]);
    return playerPosition;
}

glm::vec3 extractPlayerDirectionFromViewMatrix(const glm::mat4& viewMatrix) {
    glm::vec3 forwardDirection = glm::normalize(glm::vec3(
    -viewMatrix[0][2],  // -z_x
    -viewMatrix[1][2],  // -z_y
    -viewMatrix[2][2]   // -z_z
    ));
    return forwardDirection;
}

bool checkCollision(glm::vec3 playerPos, float playerRad, glm::vec3 objectPos, float objectRad){
	// we can ignore the y axis for collisions
	glm::vec2 playXZ = glm::vec2(playerPos.x, playerPos.z);
	glm::vec2 objXZ = glm::vec2(objectPos.x, objectPos.z);
	float distSquared = glm::length(playerPos - objectPos);
	float radSummed = playerRad + objectRad;
	float radSummedSquared = radSummed * radSummed;
	return distSquared < radSummedSquared;
}

void limitMagnitude(glm::vec2& vec, float max){
    float lengthSquared = (vec.x * vec.x + vec.y * vec.y);
    if( lengthSquared > max*max && lengthSquared > 0 ) {
        float ratio = max/(float)sqrt(lengthSquared);
        vec.x *= ratio;
        vec.y *= ratio;
    }
}
