#include "Model.h"
#include "Agent.h"
#include "Cell.h"
#include "CLI.h"
#include <iostream>
#include <chrono>

Model::Model(int h, int w, bool t, uint16_t s)
    : height(h), width(w), torus(t), rng(s) {
    grid.resize(height, std::vector<Cell>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j].initialize(this, i, j);
        }
    }
}

void Model::initializeSimulation() {
    setRunning(true);
    initializeCLI();
    loop();
}

Model::~Model() {
    setRunning(false);
}

void Model::initializeCLI() {
    cli = std::make_unique<CLI>(this);
    cli->start();
}

void Model::registerAgent(Agent* agent) {
    if (agent) {  // Check if agent is valid
        long long int id = agent->getID();
        agents[id] = std::unique_ptr<Agent>(agent);
        if (agent->getCell()) {  // Check if cell is valid
            agent->getCell()->addAgent(id);
        }
    }
}

void Model::removeAgent(long long int agentId) {
    auto it = agents.find(agentId);
    if (it != agents.end()) {
        if (it->second->getCell()) {  // Check if cell is valid
            it->second->getCell()->removeAgent(agentId);
        }
        agents.erase(it);
    }
}

void Model::queueAgentForAddition(std::unique_ptr<Agent> agent) {
    std::scoped_lock lock(agentMutex);
    agentsToAdd.push_back(std::move(agent));
}

void Model::queueAgentForRemoval(long long int agentId) {
    std::scoped_lock lock(agentMutex);
    agentsToRemove.push_back(agentId);
}

void Model::processAgentQueues() {
    // Process removals first
    for (long long int agentId : agentsToRemove) {
        removeAgent(agentId);
    }
    agentsToRemove.clear();

    // Process additions
    for (auto& agent : agentsToAdd) {
        if (agent) {  // Check if agent is still valid
            registerAgent(agent.release());
        }
    }
    agentsToAdd.clear();
}

void Model::loop()  
{
   while (simulationState.running) {  
       if (simulationState.stepOnce) {
           step();
           std::cout << "[Model] Step: " << stepCount << std::endl;
           simulationState.stepOnce = false;
       }
       if (!simulationState.playing) {
           // Wait on cv until playing is true
           std::unique_lock lk(simulationState.m);
           simulationState.cv.wait(lk);
       }  
       else {  
           step();
           std::cout << "[Model] Step: " << stepCount << std::endl;
       }  
   }  
}

void Model::step() {
    // Environmental Aspects
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j].updateEnvironment();
        }
    }
    
    // Agents Prepare/Act(Should be split up for multithreading)
    for (auto& [id, agent] : agents) {
        agent->prepare();
        agent->act();
    }

    // Then process any queued additions/removals
    processAgentQueues();

    // Increment step counter
    stepCount++;
}

void Model::shuffle_step() {
    std::vector<Agent*> agentPtrs;
    agentPtrs.reserve(agents.size());
    for (auto& [id, agent] : agents) {
        agentPtrs.push_back(agent.get());
    }

    std::shuffle(agentPtrs.begin(), agentPtrs.end(), rng);

    for (Agent* agent : agentPtrs) {
        agent->prepare();
        agent->act();
    }

    processAgentQueues();
    stepCount++;
}

void Model::display() const {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            const Cell& cell = grid[i][j];
            if (cell.hasType("Tree")) {
                std::cout << "T ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
}

void Model::collectMetrics() const {
    // Accumulate weather states
    std::unordered_map<weatherState, int> weatherCounts;
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            weatherState weather = cell.getWeather(); // Assuming getWeatherState() returns a string
            weatherCounts[weather]++;
        }
    }

    // Accumulate agent types
    std::unordered_map<std::string, int> agentCounts;
    for (const auto& [id, agent] : agents) {
        std::string type = agent->getType();
        agentCounts[type]++;
    }

    // Print metrics
    std::cout << "\n--- Metrics ---\n";
    std::cout << "Weather States:\n";
    for (const auto& [weather, count] : weatherCounts) {
        std::cout << "  " << weather << ": " << count << "\n";
    }

    std::cout << "Agent Types:\n";
    for (const auto& [type, count] : agentCounts) {
        std::cout << "  " << type << ": " << count << "\n";
    }
    std::cout << "----------------\n";
}

Agent* Model::getAgent(long long int agentId) {
    auto it = agents.find(agentId);
    if (it != agents.end()) {
        return it->second.get();
    }
    return nullptr;
}

void Model::moveAgent(long long int agentId, Cell* newCell)
{
    auto it = agents.find(agentId);
    if (it != agents.end()) {
        Agent* agent = it->second.get();
        Cell* oldCell = agent->getCell();
        if (oldCell) {
            oldCell->removeAgent(agentId);
        }
        newCell->addAgent(agentId);
        agent->setCell(newCell);
    }
}

std::mt19937& Model::getRNG() { return rng; }
long long int Model::getNextID() { return counter++; }

Cell* Model::getCell(int x, int y) {
    if (torus) {
        x = (x + height) % height;
        y = (y + width) % width;
    }
    if (x >= 0 && x < height && y >= 0 && y < width) {
        return &grid[x][y];
    }
    return nullptr;
}

void Model::registerAgentType(Agent* prototype) {
    if (!isAgentTypeInitialized(prototype->getType())) {
        prototype->initializeType();
        initializedTypes[prototype->getType()] = true;
    }
}

bool Model::isAgentTypeInitialized(const std::string& type) const {
    return initializedTypes.find(type) != initializedTypes.end();
}
