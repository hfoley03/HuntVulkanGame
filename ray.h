#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>
#include <iomanip>

// Ray is used to project a ray from the centre of the screen ie the crosshairs
// used to work out if player has shot an animal

class Ray {
    public:
        Ray(){}
        glm::vec3 getOrgin();
        glm::vec3 getDirection();
        bool rayIntersectsSphereTarget(const glm::vec3& sphereCenter, float sphereRadius);
        void calculateRayFromScreenCenter(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        void printInfo();
    
        private:
        glm::vec3 origin;
        glm::vec3 direction;
};

bool Ray::rayIntersectsSphereTarget(const glm::vec3& sphereCenter, float sphereRadius) {
    glm::vec3 L = glm::vec3(sphereCenter.x, sphereCenter.y + 0.25f, sphereCenter.z) - origin;  // ray to sph vec
    float tca = glm::dot(L, direction); // projection of L onto ray, dist along ray to closest point to sph centre
    float d2 = glm::dot(L, L) - tca * tca; //pythag
    float radius2 = sphereRadius * sphereRadius;
    return d2 <= radius2; //pythag condition met
}

void Ray::calculateRayFromScreenCenter(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

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
    origin = glm::vec3(rayStartWorld); 
    direction = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));  
}

glm::vec3 Ray::getOrgin(){ return origin;}

glm::vec3 Ray::getDirection(){ return direction;}

void Ray::printInfo(){
    std::cout << std::setw(10) << std::right << "_ _ _ Ray Information _ _ _" << "\n";
    std::cout << std::setw(10) << std::right << "_ _ _ _  Direction  _ _ _ _" << "\n";
    std::cout << "x: " << std::fixed << std::setprecision(3) << direction.x << " ";
    std::cout << "y: " << std::fixed << std::setprecision(3) << direction.y << " ";
    std::cout << "z: " << std::fixed << std::setprecision(3) << direction.z << "\n";
}