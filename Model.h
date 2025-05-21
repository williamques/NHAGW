#pragma once

#include <vector>
#include <memory>
#include <random>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "Cell.h"
#include "Agent.h"
#include "Climate.h"

class CLI;  // Forward declaration

struct SimulationState {
    std::atomic<bool> running{ false };
    std::atomic<bool> stepOnce{ false };

    std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> playing{ false };
};

class Model {
private:
    int height;
    int width;
    bool torus;
    std::vector<std::vector<Cell>> grid;
    std::unordered_map<long long int, std::unique_ptr<Agent>> agents;
    std::vector<std::unique_ptr<Agent>> agentsToAdd;
    std::vector<long long int> agentsToRemove;
    std::unique_ptr<CLI> cli;
    std::mt19937 rng;
    long long int counter = 0;
    unsigned long long stepCount = 0;
    std::unordered_map<std::string, bool> initializedTypes;

    // Threading support
    SimulationState simulationState;
    std::mutex agentMutex;  // for safely modifying agentsToAdd/agentsToRemove
    
    void registerAgent(Agent* agent);
    void removeAgent(long long int agentId);
    Climate climate;

public:
    Model(int h, int w, bool t, uint16_t s);
    void initializeSimulation();
    ~Model();

    // Agent utility functions
    void registerAgentType(Agent* prototype);
    bool isAgentTypeInitialized(const std::string& type) const;
    void queueAgentForAddition(std::unique_ptr<Agent> agent);
    void queueAgentForRemoval(long long int agentId);
    void processAgentQueues();
    Agent* getAgent(long long int agentId);
    void moveAgent(long long int agentId, Cell* newCell);

    // Update Simulation
    void loop();
    void step();
    void shuffle_step();

    // Update UI and render graphics
    void initializeCLI();
    void display() const;
    void collectMetrics() const;

    std::mt19937& getRNG();
    long long int getNextID();
    Cell* getCell(int x, int y);
    unsigned long long getStepCount() const { return stepCount; }
    bool isTorus() const { return torus; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const Climate& getClimate() const { return climate; }
    void setClimate(const Climate& newClimate) { climate = newClimate; }
    void setPlaying(bool play) { simulationState.playing = play; }
    void setRunning(bool run) {
        simulationState.running = run; 
        if (!run) {
            std::unique_lock<std::mutex> lk(simulationState.m);
            simulationState.cv.notify_all(); // Wake up the loop if it's waiting
        }
    }
    void stepOnce() { simulationState.stepOnce = true; }
    bool isRunning() const { return simulationState.running; }
    void wake() {
        std::unique_lock<std::mutex> lock(simulationState.m);
        simulationState.cv.notify_all();
    };
};
