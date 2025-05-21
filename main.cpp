#include <random>
#include "Model.h"
#include "Tree.h"
#include "Worm.h"
#include "Bird.h"


int main() {
    int height, width;
    height = 10;
    width = 10;
    int n_trees = 100;
    int n_worms = 1000;  // Number of initial worms
    int n_birds = 50;   // Number of initial birds
    uint16_t seed = 42; // Seed for RNG
    Model model(height, width, true, seed);

    // Place trees randomly
    std::uniform_int_distribution<int> dist_h(0, height - 1);
    std::uniform_int_distribution<int> dist_w(0, width - 1);
    
    // Place trees
    for (int i = 0; i < n_trees; ++i) {
        int r = dist_h(model.getRNG());
        int c = dist_w(model.getRNG());
        Cell* cell = model.getCell(r, c);
        if (cell) {
            std::unique_ptr<Tree> tree = std::make_unique<Tree>(&model, model.getNextID(), cell);
            model.queueAgentForAddition(std::move(tree));
        }
    }

    // Place worms randomly
    for (int i = 0; i < n_worms; ++i) {
        int r = dist_h(model.getRNG());
        int c = dist_w(model.getRNG());
        Cell* cell = model.getCell(r, c);
        if (cell) {
            std::unique_ptr<Worm> worm = std::make_unique<Worm>(&model, model.getNextID(), cell);
            model.queueAgentForAddition(std::move(worm));
        }
    }

    // Place birds randomly
    for (int i = 0; i < n_birds; ++i) {
        int r = dist_h(model.getRNG());
        int c = dist_w(model.getRNG());
        Cell* cell = model.getCell(r, c);
        if (cell) {
            std::unique_ptr<Bird> bird = std::make_unique<Bird>(&model, model.getNextID(), cell);
            model.queueAgentForAddition(std::move(bird));
        }
    }

    // Process all queued agents
    model.processAgentQueues();

    // Main loop
    model.initializeSimulation();

    return 0;
}