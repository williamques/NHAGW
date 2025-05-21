#ifndef TREE_H
#define TREE_H

#include "Agent.h"
#include "AgentPropertyMap.h"

class Tree : public Agent {
private:
    int age;
    int health;

public:
    Tree(Model* model, long long int id, Cell* associated_cell);

    void grow();
    void reproduce();
    void die();
    void prepare() override;
    void act() override;

    // Getters for GUI
    int getAge() const { return age; }
    int getHealth() const { return health; }

    // Virtual initialization function
    void initializeType() override {
        // Register color
        //AgentColorMap::registerColor("Tree", sf::Color(34, 139, 34)); // Forest green
            
        // Register properties
        AgentPropertyMap::registerProperty("Tree", "Age", 
            [](const Agent* agent) {
                const Tree* tree = dynamic_cast<const Tree*>(agent);
                return std::to_string(tree ? tree->getAge() : 0);
            });
            
        AgentPropertyMap::registerProperty("Tree", "Health", 
            [](const Agent* agent) {
                const Tree* tree = dynamic_cast<const Tree*>(agent);
                return std::to_string(tree ? tree->getHealth() : 0);
            });
    }
};

#endif
