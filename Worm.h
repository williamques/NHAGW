#pragma once

#include "Agent.h"

class Worm : public Agent {
private:
    int energy;
    int age;
    int maxEnergy;
    int reproductionThreshold;
    bool burrowed;

public:
    Worm(Model* model_ptr, int id, Cell* associated_cell);
    
    void prepare() override;
    void act() override;
    void initializeType() override;

    void eat();
    void move();
    void reproduce();
    void ageAndDie();

    bool isBurrowed() const { return burrowed; };
}; 