#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>
#include <string>
#include <../utils.hpp>

class Boid {
    public:
        Boid(int id);
        void run(std::vector<Boid> boids);
        void printInfo();
        glm::vec3 getPosition3D(float deltaT);
        float getAngle() { return angle;}

    private:
        glm::vec2 separate(std::vector<Boid> boids);
        glm::vec2 align(std::vector<Boid> boids);
        glm::vec2 cohesion(std::vector<Boid> boids);
        void applyForce(glm::vec2 force);
        void applyFlockForces(std::vector<Boid> boids);
        void update();
        glm::vec2 seek(glm::vec2 target);
        void bordersBounceBack();

        float mass = 10;
        float maxForce = 0.05f;
        float maxSpeed = 0.25f;
        float desiredSeparation = 2.25f;
        float align_region = 2.5f;
        float cohesion_region = 3.0;
        glm::vec2 pos, vel, acc;
        float angle = 0.0f;
        float height = 8.0f;
        int index;
        float maxFlightDistance = 30.0f;
        int maxSpawnDistance = 20;
        float ddd = 0.0f;


};

Boid::Boid(int id){
    index = id;
    acc = glm::vec2(0,0);
    vel = glm::vec2(randomFloat(-1*maxSpeed, maxSpeed), 
                        randomFloat(-1*maxSpeed, maxSpeed));

    pos =  glm::vec2(randomFloat(-1*maxSpawnDistance, maxSpawnDistance),
                        randomFloat(-1*maxSpawnDistance, maxSpawnDistance));
}

void Boid::run(std::vector<Boid> boids){
    applyFlockForces(boids);
    update();
    bordersBounceBack();
}

void Boid::applyForce(glm::vec2 force){
    glm::vec2 f = force / mass;
    acc = f + acc;
}

void Boid::applyFlockForces(std::vector<Boid> boids){
    applyForce( separate(boids) * 4.0f );
    applyForce( align(boids) * 0.25f );
    applyForce( cohesion(boids) * 0.5f );
}


void Boid::update()
{
    vel += acc;
    limitMagnitude(vel, maxSpeed);
    pos += vel;
    acc = glm::vec2(0, 0);
    if(glm::length(vel) > 0.03f){
        angle = atan2(vel.x, vel.y);
    }
}

glm::vec2 Boid::seek(glm::vec2 target)
{
    glm::vec2 desired = target - pos;
    glm::normalize(desired);
    desired *= maxSpeed;

    glm::vec2 steer = desired - vel;
    limitMagnitude(steer, maxForce);
    return steer;
}

void Boid::bordersBounceBack()
{
    if (pos.x < -maxFlightDistance){
        vel.x = vel.x * -1;
        pos.x = -maxFlightDistance;
    }
    if (pos.x > maxFlightDistance){
        vel.x = vel.x * -1;
        pos.x = maxFlightDistance;
    }
    if (pos.y < -maxFlightDistance){
        vel.y = vel.y * -1;
        pos.y = -maxFlightDistance;
    }
    if (pos.y > +maxFlightDistance){
        vel.y = vel.y * -1;
        pos.y = maxFlightDistance;
    }
}

glm::vec2 Boid::align(std::vector<Boid> boids)
{
    glm::vec2 sum = glm::vec2(0.0f, 0.0f);
    int count = 0;
    for (int i = 0; i < boids.size(); i++)
    {
        float d = glm::distance(pos, boids[i].pos);
        if ((d > 0) && (d < align_region))
        {
           sum += boids[i].vel;
           count++;
         }
    }

    if (count > 0) {
        sum = sum / static_cast<float>(count);
        glm::normalize(sum);
        sum *= maxSpeed;
        glm::vec2 steer = sum - vel;
        limitMagnitude(steer, maxForce);
        return steer;
    } else {
        return glm::vec2(0, 0);
    }
}

glm::vec2 Boid::separate(std::vector<Boid> boids)
{
    glm::vec2 steer =  glm::vec2(0.0f, 0.0f);
    int count = 0;
    for (int i = 0; i < boids.size(); i++){

        if( index == boids[i].index){
            continue;
        }

        float d = glm::distance(pos, boids[i].pos);
        if ((d > 0) && (d < desiredSeparation)) {
            glm::vec2  diff = pos - boids[i].pos;
            glm::normalize(diff);
            diff = diff/d;
            steer = steer + diff;
            count++;
         }
    }

    if (count > 0) {
        steer = steer / static_cast<float>(count);
    }
    
    if(glm::length(steer) > 0.0f){
        glm::normalize(steer);

        steer *= maxSpeed;
        steer = steer - vel;
        limitMagnitude(steer, maxForce);
    }
    return steer;
}


glm::vec2 Boid::cohesion(std::vector<Boid> boids)
{
    glm::vec2 sum = glm::vec2(0.0f, 0.0f);
    int count = 0;
    for (int i = 0; i < boids.size(); i++)
    {
        float d = glm::distance(pos, boids[i].pos);
        if ((d > 0) && (d < cohesion_region))
        {
            sum = sum + boids[i].pos;
            count++;
         }
    }
    if (count > 0 ){
        sum = sum / static_cast<float>(count);
        return seek(sum);
    } else {
        return glm::vec2(0,0);
    }
}

glm::vec3 Boid::getPosition3D(float deltaT) {
    static float elapsedTime =static_cast<float>(index);
    elapsedTime += deltaT/10.f;     

    float ang = sin(elapsedTime * M_PI); 
    height += 1 * ang;

    return glm::vec3(pos.x, height, pos.y);
}

void Boid::printInfo(){
    std::cout << "\n";
    std::cout << "Boid " << index << " pos";
    std::cout << "x: " << std::fixed << std::setprecision(3) << pos.x << " ";
    std::cout << "y: " << std::fixed << std::setprecision(3) << pos.y << " ";

    std::cout << " ___ " << index << " vel";
    std::cout << "x: " << std::fixed << std::setprecision(3) << vel.x << " ";
    std::cout << "y: " << std::fixed << std::setprecision(3) << vel.y << " ";
}