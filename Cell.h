#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include "Climate.h"

class Agent;
class Model;

class Cell {
private:
    Model* model;
    int x, y;
    std::vector<long long int> agentIds;
    // Environment information
    weatherState weather;
    
    // On top of soil information
    int water;

    // Soil information
    int soilSaturation;
    int maxSoilSaturation;
    int nutrients;

public:
    Cell();

    void initialize(Model* m, int row, int col);
    std::vector<Cell*> getOrthogonalNeighbors();
    std::vector<Cell*> getNeighborsWithinDistance(int distance);
    Cell* getRandomNeighbor();

    void setWeather(weatherState w);
    weatherState getWeather() const;
    void updateEnvironment();
    
    int getWater() const;
    void modifyWater(int w);
    int getNutrients() const;
    void modifyNutrients(int n);

    void addAgent(long long int agentId);
    void removeAgent(long long int agentId);
    bool hasType(std::string type) const;
    
    // New methods for GUI
    int getX() const { return x; }
    int getY() const { return y; }
    const std::vector<long long int>& getAgentIds() const { return agentIds; }

    int getSoilSaturation() const { return soilSaturation; }
    void modifySoilSaturation(int s);
};
