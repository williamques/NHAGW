#include "Worm.h"
#include "Model.h"
#include "Cell.h"

Worm::Worm(Model* model_ptr, int id, Cell* associated_cell)
    : Agent(model_ptr, id, "Worm", associated_cell),
      energy(50), age(0), maxEnergy(100), reproductionThreshold(80), burrowed(false) {
}

void Worm::initializeType() {
    // Register worm-specific initialization if needed
}

void Worm::prepare() {
    // Prepare for the next step
    age++;
    energy--;

    Cell* current = getCell();
    if (current) {
        burrowed = (current->getWeather() == weatherState::Drought) || (current->getWeather() == weatherState::Sunny);
    }
}

void Worm::act() {
    // Main behavior loop
    if (energy <= 0 || age > 100) {
        // Add age as nutrients to the cell
        Cell* currentCell = getCell();
        if (currentCell) {
            currentCell->modifyNutrients(age);
        }
        model->queueAgentForRemoval(unique_id);
        return;
    }

    // Try to eat first
    eat();
    
    // If we have enough energy, try to reproduce
    if (energy >= reproductionThreshold) {
        reproduce();
    }
    
    // Move to a new cell
    move();
    
    // Age and check for death
    ageAndDie();
}

void Worm::eat() {
    // Worms eat nutrients from the soil
    Cell* currentCell = getCell();
    if (currentCell) {
        int nutrients = currentCell->getNutrients();
        if (nutrients > 0) {
            int amountEaten = std::min(10, nutrients);
            currentCell->modifyNutrients(-amountEaten);
            energy = std::min(maxEnergy, energy + amountEaten);
        }
    }
}

void Worm::move() {
    // Move to a random neighboring cell
    Cell* currentCell = getCell();
    if (currentCell) {
        Cell* newCell = currentCell->getRandomNeighbor();
        if (newCell) {
            model->moveAgent(unique_id, newCell);
            energy--; // Moving costs energy
        }
    }
}

void Worm::reproduce() {
    // Create a new worm in a neighboring cell
    Cell* currentCell = getCell();
    if (currentCell) {
        Cell* newCell = currentCell->getRandomNeighbor();
        if (newCell) {
            std::unique_ptr<Worm> offspring = std::make_unique<Worm>(model, model->getNextID(), newCell);
            model->queueAgentForAddition(std::move(offspring));
            energy -= 40; // Reproduction costs energy
        }
    }
}

void Worm::ageAndDie() {
    // Chance of death increases with age
    if (age > 50) {
        std::uniform_int_distribution<int> dist(0, 100);
        if (dist(model->getRNG()) < (age - 50)) {
            Cell* currentCell = getCell();
            if (currentCell) {
                currentCell->modifyNutrients(age);
            }
            model->queueAgentForRemoval(unique_id);
        }
    }
} 