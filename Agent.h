#ifndef AGENT_H
#define AGENT_H

#include <string>

class Model;
class Cell;

class Agent {
protected:
    Model* model;
    long long int unique_id;
    std::string type;
    Cell* cell;

public:
    Agent(Model* model_ptr, int id, const std::string& agent_type, Cell* associated_cell);
    virtual void prepare() = 0;
    virtual void act() = 0;
    virtual ~Agent() = default;

    // Virtual initialization function
    virtual void initializeType() = 0;

    void setCell(Cell* cell);
    Cell* getCell() const;
    long long int getID() const;
    std::string getType() const;
};

#endif
