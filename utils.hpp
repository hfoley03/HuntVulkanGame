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
	// we can ignore the z axis for collisions
	glm::vec2 playXZ = glm::vec2(playerPos.x, playerPos.z);
	glm::vec2 objXZ = glm::vec2(objectPos.x, objectPos.z);
	float distSquared = glm::length(playerPos - objectPos);
	float radSummed = playerRad + objectRad;
	float radSummedSquared = radSummed * radSummed;
	return distSquared < radSummedSquared;
}

struct BoundingBox {
    glm::vec3 min;  // min corner
    glm::vec3 max;  // max corner
    BoundingBox() : min(0.0f), max(0.0f) {}
    BoundingBox(const glm::vec3& minCorner, const glm::vec3& maxCorner)
        : min(minCorner), max(maxCorner) {}
};

// currently unused
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

void printVec3(glm::vec3 vecThree){
    std::cout << "Vec3 Values: " << vecThree.x << ", " << vecThree.y << ", " << vecThree.z << "\n";
}