#pragma once

#include "Agent.h"
#include "Worm.h"

class Bird : public Agent {
private:
    int energy;
    int age;
    int maxEnergy;
    int reproductionThreshold;
    int visionRange;

public:
    Bird(Model* model_ptr, int id, Cell* associated_cell);
    
    void prepare() override;
    void act() override;
    void initializeType() override;

    bool hunt();
    void move();
    void reproduce();
    void ageAndDie();
    Worm* findPrey();
    Bird* findMate();
}; 