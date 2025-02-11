#include <iostream>
#include <list>
#include "boid.h"

class Flock{

    public:
        
        Flock(int numberOfBoids);
        void run();
        void addBoid(Boid boid);
        void clearFlock();
        std::vector<Boid> getFlock();
        Boid getBoid(int index);
        void printInfo(){}

    private:
        std::vector<Boid> boids;
        glm::vec3 positions;
        int numberOfBoids = 0;
    
};

Flock::Flock(int numBoids){
    std::cout << "Flock Setup" << "\n";
    numberOfBoids = numBoids;
    for(int i = 0 ; i < numberOfBoids ; i++){
        Boid boid = Boid(i);
        boids.push_back(boid);
    }
}

void Flock::run(){
    for(auto& boid : boids){
        boid.run(boids);
    }
}

void Flock::addBoid(Boid boid){
    boids.push_back(boid);
}

Boid Flock::getBoid(int index){
    return boids[index];
}

std::vector<Boid> Flock::getFlock(){
    return boids;
}
