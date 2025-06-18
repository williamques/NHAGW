#include "Bird.h"
#include "Model.h"
#include "Cell.h"
#include <vector>
#include <algorithm>
#include <iostream>

Bird::Bird(Model* model_ptr, int id, Cell* associated_cell, Gender g)
    : Agent(model_ptr, id, "Bird", associated_cell),
      energy(200), age(0), maxEnergy(300), reproductionThreshold(150), visionRange(3),  gender(g), isCallingForMate(false), timeSpentCalling(0) {
}

void Bird::initializeType() {
    // Register bird-specific initialization if needed
}

void Bird::prepare() {
    // Prepare for the next step
    age++;
}

void Bird::act() {
    if (energy <= 0 || age > 200) {
        model->queueAgentForRemoval(unique_id);
        return;
    }

    if (reproduce()) {}
    else if (hunt()){}
    else move();

    ageAndDie();
}


bool Bird::hunt() {
    energy -= 10; // Hunting costs energy
    Worm* worm = findPrey();
    if (worm) {
        energy = std::min(maxEnergy, energy + 40);
        model->queueAgentForRemoval(worm->getID());
        return true;
        
    }
    return false; // Failed Hunt
}

void Bird::move() {
    Cell* currentCell = getCell();
    if (currentCell) {
        Cell* newCell = currentCell->getRandomNeighbor();
        if (newCell) {
            model->moveAgent(unique_id, newCell);
            energy -= 5; // Moving costs energy
        }
    }
}

void Bird::moveTowards(Cell * target) {
    Cell* currentCell = getCell();
    if (currentCell == nullptr || target == nullptr) return;
    Cell* newCell = currentCell;

    if (currentCell != target) {
        // Move towards the mate
        if (target->getX() < currentCell->getX()) {
            newCell = model->getCell(currentCell->getX() - 1, currentCell->getY());
        }
        else if (target->getX() > cell->getX()) {
            newCell = model->getCell(currentCell->getX() + 1, currentCell->getY());
        }
        else if (target->getY() < cell->getY()) {
            newCell = model->getCell(currentCell->getX(), currentCell->getY() - 1);
        }
        else if (target->getY() > cell->getY()) {
            newCell = model->getCell(currentCell->getX(), currentCell->getY() + 1);
            
        }
        if (newCell) {
            model->moveAgent(getID(), newCell);
        }
    }
}

bool Bird::reproduce() {
    // Returns true if progress towards finding a mate was successful
    Cell* currentCell = getCell();
    if (!currentCell) return false;
    if (energy < reproductionThreshold) {
        return false;
    }
    if (gender == Gender::Female) {
        if (timeSpentCalling > visionRange) {
            // Will stop calling for mate after visionRange + 1 steps unless blind
            isCallingForMate = false;
            timeSpentCalling = 0;
            return false;
        }
        // Perform mating call
        isCallingForMate = true;
        timeSpentCalling++;
        
        return true;
    }

    Bird* mate = findMate();
    if (mate == nullptr) {
        return false;
    }

    Cell* mateCell = mate->getCell();
    // If mate is not in current sell
    if (currentCell != mateCell) {
        moveTowards(mateCell);
        return true;
    }

    // If mate is in current cell
    if (currentCell == mateCell && mate->energy >= reproductionThreshold ) {
        // Both parents lose energy
        energy -= 50;
        mate->energy -= 50;

        // Place offspring in current cell
        std::unique_ptr<Bird> offspring;
        if (model->getRNG()() % 2 < 1) {
            offspring = std::make_unique<Bird>(model, model->getNextID(), currentCell, Gender::Male);
        }
        else {
            offspring = std::make_unique<Bird>(model, model->getNextID(), currentCell, Gender::Female);
        }
        model->queueAgentForAddition(std::move(offspring));
        return true;
    }
    return false;
}

void Bird::ageAndDie() {
    // Chance of death increases with age
    if (age > 10) {
        std::uniform_int_distribution<int> dist(0, 10);
        if (dist(model->getRNG()) < (age - 10)) {
            model->queueAgentForRemoval(unique_id);
        }
    }
}

Worm* Bird::findPrey() {
    // Look for prey in current cell
    Cell* currentCell = getCell();
    if (!currentCell) return nullptr;

    const std::vector<long long int>& agentIds = currentCell->getAgentIds();
    for (long long int agentId : agentIds) {
        Agent* agent = model->getAgent(agentId);
        if (agent && agent->getType() == "Worm") {
            Worm* prey = dynamic_cast<Worm*>(agent);
            if (prey && !prey->isBurrowed()) {
                return prey;
            }
        }
    }
    return nullptr;
} 

Bird* Bird::findMate() {
    // Get all neighboring cells within visionRange
    Cell* currentCell = getCell();
    if (!currentCell) return nullptr;

    std::vector<Cell*> neighbors;
    neighbors = cell->getNeighborsWithinDistance(visionRange);

    for (Cell* neighbor : neighbors) {
        for (long long int agentId : neighbor->getAgentIds()) {
            Agent* agent = model->getAgent(agentId);
            if (agent->getType() == "Bird") {
                Bird* otherBird = dynamic_cast<Bird*>(agent);
                if (otherBird &&
                    otherBird != this &&
                    otherBird->getGender() != this->getGender() &&
                    otherBird->isMakingMatingCall()) {
                    return otherBird;
                }
            }
        }
    }
    return nullptr;
}

bool Bird::isMakingMatingCall() const {
    return isCallingForMate;
}

Bird::Gender Bird::getGender() const
{
    return gender;
}
