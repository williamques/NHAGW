#include "Agent.h"
#include "Cell.h"

Agent::Agent(Model* model_ptr, int id, const std::string& agent_type, Cell* associated_cell)
    : model(model_ptr), unique_id(id), type(agent_type), cell(associated_cell) {
}

void Agent::setCell(Cell* c) { cell = c; }
Cell* Agent::getCell() const { return cell; }
long long int Agent::getID() const { return unique_id; }
std::string Agent::getType() const { return type; }
