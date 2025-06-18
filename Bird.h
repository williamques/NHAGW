#pragma once

#include "Agent.h"
#include "Worm.h"

class Bird : public Agent {
public:
    enum class Gender { Male, Female };
private:
    int energy;
    int age;
    int maxEnergy;
    int reproductionThreshold;
    int visionRange;
    Gender gender;
    bool isCallingForMate = false;
    int timeSpentCalling = 0;

public:
    Bird(Model* model_ptr, int id, Cell* associated_cell, Gender gender);

    void prepare() override;
    void act() override;
    void initializeType() override;

    bool hunt();
    void move();
    void moveTowards(Cell* target);
    bool reproduce();
    void ageAndDie();
    Worm* findPrey();
    Bird* findMate();

    bool isMakingMatingCall() const;
    Gender getGender() const;
};
