#include "Bird.h"
#include "Model.h"
#include "Cell.h"
#include <vector>
#include <algorithm>

Bird::Bird(Model* model_ptr, int id, Cell* associated_cell)
    : Agent(model_ptr, id, "Bird", associated_cell),
      energy(100), age(0), maxEnergy(200), reproductionThreshold(150), visionRange(3) {
}

void Bird::initializeType() {
    // Register bird-specific initialization if needed
}

void Bird::prepare() {
    // Prepare for the next step
    age++;
}

void Bird::act() {
    // Main behavior loop
    if (energy <= 0 || age > 200) {
        model->queueAgentForRemoval(unique_id);
        return;
    }
    // Try to hunt first
    if (!hunt()) {
        move();
    }
    
    // If we have enough energy, try to reproduce
    if (energy >= reproductionThreshold) {
        reproduce();
    }
    
    // Age and check for death
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
    // If no prey was found, move randomly
    Cell* currentCell = getCell();
    if (currentCell) {
        Cell* newCell = currentCell->getRandomNeighbor();
        if (newCell) {
            model->moveAgent(unique_id, newCell);
            energy -= 5; // Moving costs energy
        }
    }
}

void Bird::reproduce() {
    Cell* currentCell = getCell();
    if (!currentCell) return;

    Bird* mate = findMate();
    if (mate && mate->energy >= reproductionThreshold) {
        // Both parents lose energy
        energy -= 50;
        mate->energy -= 50;

        // Place offspring in a neighboring cell (of either parent)
        Cell* newCell = currentCell->getRandomNeighbor();
        if (!newCell) newCell = mate->getCell()->getRandomNeighbor();

        if (newCell) {
            std::unique_ptr<Bird> offspring = std::make_unique<Bird>(model, model->getNextID(), newCell);
            model->queueAgentForAddition(std::move(offspring));
        }
    }
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
    // Simple implementation: look for prey in current cell
    Cell* currentCell = getCell();
    if (!currentCell) return nullptr;

    const std::vector<long long int>& agentIds = currentCell->getAgentIds();
    for (long long int agentId : agentIds) {
        Agent* agent = model->getAgent(agentId);
        if (agent && agent->getType() == "Bird") {
            Worm* prey = dynamic_cast<Worm*>(agent);
            if (prey && !prey->isBurrowed()) {
                return prey;
            }
        }
    }
    return nullptr;
} 

Bird* Bird::findMate() {
    Cell* currentCell = getCell();
    if (!currentCell) return nullptr;

    std::vector<Cell*> neighbors = currentCell->getOrthogonalNeighbors();
    for (Cell* neighbor : neighbors) {
        if (!neighbor) continue;

        const std::vector<long long int>& agentIds = neighbor->getAgentIds();
        for (long long int agentId : agentIds) {
            Agent* agent = model->getAgent(agentId);
            if (agent && agent->getType() == "Bird") {
                Bird* potentialMate = dynamic_cast<Bird*>(agent);
                if (potentialMate && potentialMate != this && potentialMate->energy >= reproductionThreshold) {
                    return potentialMate;
                }
            }
        }
    }
    return nullptr;
}