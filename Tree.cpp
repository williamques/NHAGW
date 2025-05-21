#include "Tree.h"
#include "Model.h"
#include "Cell.h"
#include "AgentPropertyMap.h"
#include <iostream>

Tree::Tree(Model* model, long long int id, Cell* associated_cell)
    : Agent(model, id, "Tree", associated_cell), age(0), health(20) {
}

void Tree::grow() {
    if (cell->getSoilSaturation() > 0 && cell->getNutrients() > 0) {
        age++;
        health += 10;
        cell->modifySoilSaturation(-1);
        cell->modifyNutrients(-1);
    }
    else if (cell->getSoilSaturation() > 0) {
        cell->modifySoilSaturation(-1);
        health -= 5;
    }
    else if (cell->getNutrients() > 0) {
        cell->modifyNutrients(-1);
        health -= 5;
    }
    else {
        health -= 20;
    }
}

void Tree::reproduce() {
    if (health > 50) {
        Cell* new_cell = cell->getRandomNeighbor();
        if (new_cell) {  // Make sure we have a valid cell
            // Create the new tree and immediately queue it for addition
            std::unique_ptr<Tree> offspring = std::make_unique<Tree>(model, model->getNextID(), new_cell);
            model->queueAgentForAddition(std::move(offspring));
            health -= 30;
        }
    }
}

void Tree::die() {
    if (health <= 0) {
        cell->modifyNutrients(age / 5);
        model->queueAgentForRemoval(unique_id);
    }
}

void Tree::prepare() {
    return;
}

void Tree::act() {
    grow();
    reproduce();
    die();
}
